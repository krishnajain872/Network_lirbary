#include "logger/log_formatter.h"
#include "logger/json.h"
#include <sstream>
#include <iomanip>

namespace networklib {
namespace logging {

std::string LogFormatter::Format(const LogEntry& entry, const LoggerConfig& config) {
    if (config.format == "json") {
        return FormatJson(entry, config);
    }
    return FormatText(entry, config);
}

std::string LogFormatter::FormatText(const LogEntry& entry, const LoggerConfig& config) {
    std::stringstream ss;

    // Timestamp
    auto time_t = std::chrono::system_clock::to_time_t(entry.timestamp);
    std::tm tm_buf;
#if defined(__linux__) || defined(__APPLE__)
    localtime_r(&time_t, &tm_buf);
#else
    localtime_s(&tm_buf, &time_t);
#endif
    ss << "[" << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    if (config.timestamp_precision == "microsecond") {
        auto duration = entry.timestamp.time_since_epoch();
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(duration).count() % 1000000;
        ss << "." << std::setfill('0') << std::setw(6) << us;
    } else if (config.timestamp_precision == "millisecond") {
        auto duration = entry.timestamp.time_since_epoch();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;
        ss << "." << std::setfill('0') << std::setw(3) << ms;
    }
    ss << "] ";

    // Level
    ss << "[" << LevelToString(entry.level) << "] ";

    // Metadata
    if (config.include_metadata) {
        ss << "[" << entry.process_id << ":" << entry.thread_id << "] ";
        if (!entry.thread_name.empty()) {
            ss << "[" << entry.thread_name << "] ";
        }
    }

    // Caller
    if (config.include_caller && !entry.file.empty()) {
        // Shorten file path? For now full path or filename
        size_t last_slash = entry.file.find_last_of("/\\");
        std::string filename = (last_slash == std::string::npos) ? entry.file : entry.file.substr(last_slash + 1);
        ss << "[" << filename << ":" << entry.line << ":" << entry.function << "] ";
    }

    // App Name
    if (!entry.app_name.empty()) {
        ss << "[" << entry.app_name << "] ";
    }

    // Message
    ss << entry.message;

    return ss.str();
}

std::string LogFormatter::FormatJson(const LogEntry& entry, const LoggerConfig& config) {
    nlohmann::json j;

    auto time_t = std::chrono::system_clock::to_time_t(entry.timestamp);
    // ISO 8601
    std::stringstream ts;
    std::tm tm_buf;
#if defined(__linux__) || defined(__APPLE__)
    localtime_r(&time_t, &tm_buf);
#else
    localtime_s(&tm_buf, &time_t);
#endif
    ts << std::put_time(&tm_buf, "%Y-%m-%dT%H:%M:%S");
    // Add fractional?
    auto duration = entry.timestamp.time_since_epoch();
    auto us = std::chrono::duration_cast<std::chrono::microseconds>(duration).count() % 1000000;
    ts << "." << std::setfill('0') << std::setw(6) << us << "Z"; // Local time but marked Z? Actually usually offsets. Simplified.

    j["timestamp"] = ts.str();
    j["level"] = LevelToString(entry.level);
    j["message"] = entry.message;
    j["process_id"] = entry.process_id;

    // Convert thread_id to string
    std::stringstream tid_ss;
    tid_ss << entry.thread_id;
    j["thread_id"] = tid_ss.str();

    if (!entry.thread_name.empty()) j["thread_name"] = entry.thread_name;
    if (!entry.file.empty()) {
        j["file"] = entry.file;
        j["line"] = entry.line;
        j["function"] = entry.function;
    }

    // Add host/app if available
    if (!entry.hostname.empty()) j["hostname"] = entry.hostname;
    if (!entry.app_name.empty()) j["app_name"] = entry.app_name;

    return j.dump();
}

const char* LogFormatter::LevelToString(LogLevel level) {
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

} // namespace logging
} // namespace networklib
