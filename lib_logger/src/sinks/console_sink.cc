#include "logger/sinks/console_sink.h"
#include "logger/log_formatter.h"
#include <iostream>

namespace networklib {
namespace logging {

ConsoleSink::ConsoleSink(const LoggerConfig& config) : config_(config) {}

void ConsoleSink::Write(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::cout << LogFormatter::Format(entry, config_) << std::endl;
}

void ConsoleSink::Flush() {
    std::cout.flush();
}

} // namespace logging
} // namespace networklib
