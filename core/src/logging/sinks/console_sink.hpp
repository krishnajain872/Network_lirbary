#pragma once

#include "log_sink.hpp"
#include <iostream>
#include <mutex>

namespace networklib {
namespace logging {

class ConsoleSink : public LogSink {
public:
    void Write(const LogEntry& entry) override {
        // Simple console output for now, can be colorized later
        std::lock_guard<std::mutex> lock(mutex_);

        // Format: [Timestamp] [Level] [Thread] Message
        // Using a very simple format for the base implementation
        auto time_t = std::chrono::system_clock::to_time_t(entry.timestamp);
        std::cout << "[" << std::put_time(std::localtime(&time_t), "%Y-%m-%dT%H:%M:%S") << "] "
                  << "[" << LogLevelToString(entry.level) << "] "
                  << "[" << entry.thread_name << "] "
                  << entry.message << std::endl;
    }

    void Flush() override {
        std::cout.flush();
    }

private:
    std::mutex mutex_;

    const char* LogLevelToString(LogLevel level) {
        switch (level) {
            case LogLevel::Trace: return "TRACE";
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warn: return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Fatal: return "FATAL";
            case LogLevel::Critical: return "CRITICAL";
            default: return "UNKNOWN";
        }
    }
};

} // namespace logging
} // namespace networklib
