#pragma once

#include "log_sink.hpp"
#include "../log_formatter.hpp"
#include <iostream>
#include <mutex>

namespace networklib {
namespace logging {

class ConsoleSink : public LogSink {
public:
    explicit ConsoleSink(const LoggerConfig& config) : config_(config) {}

    void Write(const LogEntry& entry) override {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << LogFormatter::Format(entry, config_) << std::endl;
    }

    void Flush() override {
        std::cout.flush();
    }

private:
    LoggerConfig config_;
    std::mutex mutex_;
};

} // namespace logging
} // namespace networklib
