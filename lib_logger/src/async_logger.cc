#include "async_logger.h"
#include "sinks/console_sink.h"
#include "sinks/file_sink.h"
#include "sinks/syslogger/sinks/log_sink.h"
#include "sinks/kafka_sink.h"

namespace networklib {
namespace logging {

AsyncLogger::AsyncLogger() : running_(false) {}

AsyncLogger::~AsyncLogger() {
    Shutdown();
}

void AsyncLogger::Initialize(const LoggerConfig& config) {
    config_ = config;

    // Add default sinks based on config
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
    if (!running_) return;

    running_ = false;
    queue_cv_.notify_all();

    if (worker_.joinable()) {
        worker_.join();
    }

    // Flush remaining logs
    while (!queue_.empty()) {
        LogEntry entry = std::move(queue_.front());
        queue_.pop();
        for (auto& sink : sinks_) {
            sink->Write(entry);
        }
    }

    for (auto& sink : sinks_) {
        sink->Flush();
    }
    sinks_.clear();
}

void AsyncLogger::Enqueue(LogEntry&& entry) {
    if (!running_) return;

    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        if (queue_.size() >= config_.async_queue_size) {
            // Drop or block? For now, drop to avoid blocking app
            return;
        }
        queue_.push(std::move(entry));
    }
    queue_cv_.notify_one();
}

void AsyncLogger::AddSink(std::unique_ptr<LogSink> sink) {
    sinks_.push_back(std::move(sink));
}

void AsyncLogger::WorkerThread() {
    while (running_) {
        LogEntry entry;
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            queue_cv_.wait(lock, [this] { return !queue_.empty() || !running_; });

            if (!running_ && queue_.empty()) break;

            if (queue_.empty()) continue;

            entry = std::move(queue_.front());
            queue_.pop();
        }

        // Write to all sinks
        for (auto& sink : sinks_) {
            sink->Write(entry);
        }
    }
}

} // namespace logging
} // namespace networklib
