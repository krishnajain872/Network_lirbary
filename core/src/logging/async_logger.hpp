#pragma once

#include "log_common.hpp"
#include "sinks/log_sink.hpp"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <vector>
#include <memory>

namespace networklib {
namespace logging {

class AsyncLogger {
public:
    AsyncLogger();
    ~AsyncLogger();

    void Initialize(const LoggerConfig& config);
    void Shutdown();

    void Enqueue(LogEntry&& entry);

    // Add sinks dynamically
    void AddSink(std::unique_ptr<LogSink> sink);

private:
    void WorkerThread();

    LoggerConfig config_;
    std::vector<std::unique_ptr<LogSink>> sinks_;

    // Queue
    std::queue<LogEntry> queue_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;

    std::atomic<bool> running_;
    std::thread worker_;
};

} // namespace logging
} // namespace networklib
