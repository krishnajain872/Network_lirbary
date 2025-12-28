#include "logger/async_logger.h"
#include "logger/sinks/console_sink.h"
#include "logger/sinks/file_sink.h"
#include "logger/sinks/kafka_sink.h"
#include "logger/sinks/syslog_sink.h"
#include <algorithm>


namespace networklib {
namespace logging {

AsyncLogger::AsyncLogger()
    : running_(false), overflow_policy_(OverflowPolicy::BLOCK),
      dropped_count_(0), processed_count_(0) {}

AsyncLogger::~AsyncLogger() { Shutdown(); }

void AsyncLogger::Initialize(const LoggerConfig &config) {
  config_ = config;

  // Set overflow policy from config
  if (config_.overflow_policy == "block") {
    overflow_policy_ = OverflowPolicy::BLOCK;
  } else if (config_.overflow_policy == "discard_oldest") {
    overflow_policy_ = OverflowPolicy::DISCARD_OLDEST;
  } else {
    overflow_policy_ = OverflowPolicy::DISCARD_NEWEST;
  }

  // Add sinks based on config
  if (config_.console_enabled) {
    AddSink(std::make_unique<ConsoleSink>(config_));
  }

  if (config_.file_enabled && !config_.file_path.empty()) {
    AddSink(std::make_unique<FileSink>(config_));
  }

  if (config_.syslog_enabled) {
    AddSink(std::make_unique<SyslogSink>(config_));
  }

  if (config_.kafka_enabled) {
    AddSink(std::make_unique<KafkaSink>(config_));
  }

  running_ = true;
  worker_ = std::thread(&AsyncLogger::WorkerThread, this);
}

void AsyncLogger::Shutdown() {
  if (!running_.exchange(false)) {
    return; // Already shut down
  }

  // Wake up worker thread
  queue_cv_.notify_all();

  if (worker_.joinable()) {
    worker_.join();
  }

  // Process remaining logs in queue
  std::vector<LogEntry> remaining;
  {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    while (!queue_.empty()) {
      remaining.push_back(std::move(queue_.front()));
      queue_.pop();
    }
  }

  if (!remaining.empty()) {
    ProcessBatch(remaining);
  }

  // Flush and clear all sinks
  for (auto &sink : sinks_) {
    sink->Flush();
  }
  sinks_.clear();
}

bool AsyncLogger::Enqueue(LogEntry &&entry) {
  if (!running_) {
    return false;
  }

  std::unique_lock<std::mutex> lock(queue_mutex_);
  return TryEnqueue(std::move(entry), lock);
}

bool AsyncLogger::EnqueueWithTimeout(LogEntry &&entry,
                                     std::chrono::milliseconds timeout) {
  if (!running_) {
    return false;
  }

  std::unique_lock<std::mutex> lock(queue_mutex_);

  // If queue is full, wait for space
  if (queue_.size() >= config_.async_queue_size) {
    if (overflow_policy_ == OverflowPolicy::BLOCK) {
      auto deadline = std::chrono::steady_clock::now() + timeout;
      if (!space_cv_.wait_until(lock, deadline, [this] {
            return queue_.size() < config_.async_queue_size || !running_;
          })) {
        // Timeout occurred
        dropped_count_++;
        return false;
      }

      if (!running_) {
        return false;
      }
    }
  }

  return TryEnqueue(std::move(entry), lock);
}

bool AsyncLogger::TryEnqueue(LogEntry &&entry,
                             std::unique_lock<std::mutex> &lock) {
  if (queue_.size() >= config_.async_queue_size) {
    switch (overflow_policy_) {
    case OverflowPolicy::BLOCK:
      // Wait for space (caller should use EnqueueWithTimeout instead)
      space_cv_.wait(lock, [this] {
        return queue_.size() < config_.async_queue_size || !running_;
      });

      if (!running_) {
        return false;
      }
      break;

    case OverflowPolicy::DISCARD_OLDEST:
      // Remove oldest entry to make space
      queue_.pop();
      dropped_count_++;
      break;

    case OverflowPolicy::DISCARD_NEWEST:
      // Drop the new entry
      dropped_count_++;
      return false;
    }
  }

  queue_.push(std::move(entry));
  queue_cv_.notify_one();
  return true;
}

void AsyncLogger::AddSink(std::unique_ptr<LogSink> sink) {
  sinks_.push_back(std::move(sink));
}

void AsyncLogger::SetOverflowPolicy(OverflowPolicy policy) {
  overflow_policy_ = policy;
}

size_t AsyncLogger::GetQueueSize() const {
  std::lock_guard<std::mutex> lock(queue_mutex_);
  return queue_.size();
}

size_t AsyncLogger::GetDroppedCount() const { return dropped_count_.load(); }

size_t AsyncLogger::GetProcessedCount() const {
  return processed_count_.load();
}

void AsyncLogger::WorkerThread() {
  std::vector<LogEntry> batch;
  batch.reserve(BATCH_SIZE);

  while (running_) {
    {
      std::unique_lock<std::mutex> lock(queue_mutex_);

      // Wait for entries or timeout
      queue_cv_.wait_for(lock, BATCH_TIMEOUT,
                         [this] { return !queue_.empty() || !running_; });

      if (!running_ && queue_.empty()) {
        break;
      }

      // Collect batch
      while (!queue_.empty() && batch.size() < BATCH_SIZE) {
        batch.push_back(std::move(queue_.front()));
        queue_.pop();
      }
    }

    // Notify waiters that space is available
    if (!batch.empty()) {
      space_cv_.notify_all();
    }

    // Process batch outside of lock
    if (!batch.empty()) {
      ProcessBatch(batch);
      batch.clear();
    }
  }
}

void AsyncLogger::ProcessBatch(std::vector<LogEntry> &batch) {
  // Write batch to all sinks
  for (auto &sink : sinks_) {
    // Some sinks might support batch operations
    for (auto &entry : batch) {
      try {
        sink->Write(entry);
      } catch (const std::exception &e) {
        // Log sink error to stderr (avoid recursion)
        // In production, consider a separate error handler
      }
    }
  }

  processed_count_ += batch.size();
}

} // namespace logging
} // namespace networklib