#pragma once

#include "logger/log_common.h"
#include <string>

namespace networklib {
namespace logging {

class LogFormatter {
public:
    static std::string Format(const LogEntry& entry, const LoggerConfig& config);

private:
    static std::string FormatText(const LogEntry& entry, const LoggerConfig& config);
    static std::string FormatJson(const LogEntry& entry, const LoggerConfig& config);
    static const char* LevelToString(LogLevel level);
};

} // namespace logging
} // namespace networklib
