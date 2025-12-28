#include "logger/log_common.h"
#include <sstream>
#include <iostream>
#include <algorithm>

namespace networklib {
namespace logging {

// Helper to split string by delimiter
static std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

static LogLevel StringToLogLevel(const std::string& level) {
    std::string s = level;
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    if (s == "TRACE") return LogLevel::Trace;
    if (s == "DEBUG") return LogLevel::Debug;
    if (s == "INFO") return LogLevel::Info;
    if (s == "WARN") return LogLevel::Warn;
    if (s == "ERROR") return LogLevel::Error;
    if (s == "FATAL") return LogLevel::Fatal;
    if (s == "CRITICAL") return LogLevel::Critical;
    return LogLevel::Info;
}

static size_t ParseBytes(const std::string& s) {
    // Basic parsing, e.g., "100MB" -> bytes
    size_t multiplier = 1;
    std::string num_part = s;
    if (s.size() > 2) {
        std::string suffix = s.substr(s.size() - 2);
        std::transform(suffix.begin(), suffix.end(), suffix.begin(), ::toupper);
        if (suffix == "KB") { multiplier = 1024; num_part = s.substr(0, s.size() - 2); }
        else if (suffix == "MB") { multiplier = 1024 * 1024; num_part = s.substr(0, s.size() - 2); }
        else if (suffix == "GB") { multiplier = 1024 * 1024 * 1024; num_part = s.substr(0, s.size() - 2); }
        else if (s.back() == 'B') { num_part = s.substr(0, s.size() - 1); }
    }
    try {
        return std::stoull(num_part) * multiplier;
    } catch (...) {
        return 100 * 1024 * 1024; // Default
    }
}

static std::chrono::milliseconds ParseDuration(const std::string& s) {
    // Basic parsing, e.g., "1000ms"
    long long multiplier = 1; // milliseconds
    std::string num_part = s;
    if (s.size() > 2 && s.substr(s.size()-2) == "ms") {
        num_part = s.substr(0, s.size() - 2);
    } else if (s.back() == 's') {
        multiplier = 1000;
        num_part = s.substr(0, s.size() - 1);
    }
    try {
        return std::chrono::milliseconds(std::stoll(num_part) * multiplier);
    } catch (...) {
        return std::chrono::milliseconds(1000);
    }
}

LoggerConfig ParseConfig(const char* config_string) {
    LoggerConfig config;
    if (!config_string) return config;

    std::string str(config_string);
    auto pairs = split(str, ';');

    for (const auto& pair : pairs) {
        auto kv = split(pair, '=');
        if (kv.size() != 2) continue;
        std::string key = kv[0];
        std::string value = kv[1];

        // Trim
        // (Assuming input is reasonably clean, skipping rigorous trim for brevity)

        if (key == "appname") config.app_name = value;
        else if (key == "thread_name") config.thread_name = value;
        else if (key == "console") config.console_enabled = (value == "true");
        else if (key == "logfile") { config.file_enabled = true; config.file_path = value; }
        else if (key == "syslog") config.syslog_enabled = (value == "true");
        else if (key == "kafka") config.kafka_enabled = (value == "true");
        else if (key == "kafka_broker") config.kafka_broker = value;
        else if (key == "kafka_topic") config.kafka_topic = value;
        else if (key == "kafka_partition") try { config.kafka_partition = std::stoi(value); } catch(...) {}
        else if (key == "kafka_compression") config.kafka_compression = value;
        else if (key == "syslog_facility") config.syslog_facility = value;
        else if (key == "syslog_server") config.syslog_server = value;
        else if (key == "rotation_size") config.rotation_size = ParseBytes(value);
        else if (key == "rotation_time") config.rotation_time = value;
        else if (key == "max_backups") try { config.max_backups = std::stoi(value); } catch(...) {}
        else if (key == "rotation_compress") config.rotation_compress = (value == "true");
        else if (key == "severity") config.severity = StringToLogLevel(value);
        else if (key == "format") config.format = value;
        else if (key == "timestamp_precision") config.timestamp_precision = value;
        else if (key == "include_metadata") config.include_metadata = (value == "true");
        else if (key == "include_caller") config.include_caller = (value == "true");
        else if (key == "async") config.async = (value == "true");
        else if (key == "async_queue_size") try { config.async_queue_size = std::stoull(value); } catch(...) {}
        else if (key == "flush_interval") config.flush_interval = ParseDuration(value);
        else if (key == "buffer_size") try { config.buffer_size = std::stoull(value); } catch(...) {}
        else if (key == "batch_size") try { config.batch_size = std::stoull(value); } catch(...) {}
        else if (key == "overflow_policy") config.overflow_policy = value;
    }
    return config;
}

} // namespace logging
} // namespace networklib
