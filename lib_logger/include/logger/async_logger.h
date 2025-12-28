#pragma once

#include "logger_config.h"
#include "sinks/log_sink.h"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace networklib {
namespace logging {

enum class OverflowPolicy {
  BLOCK,          // Block caller until space available
  DISCARD_OLDEST, // Drop oldest log entries
  DISCARD_NEWEST  // Drop newest log entries (default drop behavior)
};
class AsyncLogger {
public:
  AsyncLogger();
  ~AsyncLogger();

  // Prevent copying
  AsyncLogger(const AsyncLogger &) = delete;
  AsyncLogger &operator=(const AsyncLogger &) = delete;

  void Initialize(const LoggerConfig &config);
  void Shutdown();

  // Non-blocking enqueue with configurable overflow policy
  bool Enqueue(LogEntry &&entry);

  // Blocking enqueue with timeout
  bool EnqueueWithTimeout(LogEntry &&entry, std::chrono::milliseconds timeout);

  void AddSink(std::unique_ptr<LogSink> sink);
  void SetOverflowPolicy(OverflowPolicy policy);

  // Statistics
  size_t GetQueueSize() const;
  size_t GetDroppedCount() const;
  size_t GetProcessedCount() const;

private:
  void WorkerThread();
  void ProcessBatch(std::vector<LogEntry> &batch);
  bool TryEnqueue(LogEntry &&entry, std::unique_lock<std::mutex> &lock);

  LoggerConfig config_;
  std::vector<std::unique_ptr<LogSink>> sinks_;

  // Lock-free queue alternative: use std::queue with mutex for now
  // For high-performance: consider boost::lockfree::spsc_queue
  std::queue<LogEntry> queue_;
  mutable std::mutex queue_mutex_;
  std::condition_variable queue_cv_;
  std::condition_variable space_cv_; // For blocking on full queue

  std::thread worker_;
  std::atomic<bool> running_;

  // Overflow handling
  OverflowPolicy overflow_policy_;

  // Statistics
  std::atomic<size_t> dropped_count_;
  std::atomic<size_t> processed_count_;

  // Batch processing
  static constexpr size_t BATCH_SIZE = 32;
  static constexpr auto BATCH_TIMEOUT = std::chrono::milliseconds(100);
};

} // namespace logging
} // namespace networklib