#pragma once

#include "networklib/network_lib.h"
#include "networklib/external/json.hpp"
#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include <unordered_map>

namespace networklib {
namespace logging {

struct LogEntry {
    std::chrono::system_clock::time_point timestamp;
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::thread::id thread_id;
    std::string thread_name;
    int process_id;
    std::string hostname;
    std::string app_name;
    unsigned long sequence;
};

struct LoggerConfig {
    // Identity
    std::string app_name;
    std::string thread_name; // Default, can be overridden per thread

    // Sinks
    bool console_enabled = true;
    bool file_enabled = false;
    std::string file_path;
    bool syslog_enabled = false;
    bool kafka_enabled = false;

    // Kafka
    std::string kafka_broker;
    std::string kafka_topic;
    int kafka_partition = -1;
    std::string kafka_compression = "none";

    // Syslog
    std::string syslog_facility = "LOCAL0";
    std::string syslog_server;

    // Rotation
    size_t rotation_size = 100 * 1024 * 1024; // 100MB
    std::string rotation_time = "24h";
    int max_backups = 10;
    bool rotation_compress = false;

    // Filtering
    LogLevel severity = LogLevel::Info;

    // Format
    std::string format = "text"; // text, json, structured
    std::string timestamp_precision = "microsecond";
    bool include_metadata = true;
    bool include_caller = true;

    // Async
    bool async = true;
    size_t async_queue_size = 100000;
    std::chrono::milliseconds flush_interval{1000};
    size_t buffer_size = 4096;
    size_t batch_size = 100;
};

LoggerConfig ParseConfig(const char* config_string);

} // namespace logging
} // namespace networklib
