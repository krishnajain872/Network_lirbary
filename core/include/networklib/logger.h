#pragma once

#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>
#include <atomic>
#include <sstream>
#include <iostream>
#include <cstring>

namespace networklib {
namespace logging {

// Severity Levels
enum class LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    CRITICAL
};

// Main Logger API
class Logger {
public:
    // Initialize with semicolon-separated config string
    static bool Initialize(const char* config_string);

    // Main logging function
    static void Log(LogLevel level, const char* file, int line, const char* func, const char* format, ...);

    // Cleanup and flush
    static void Deinitialize();

    // Check if a level is enabled (for fast skipping)
    static bool IsLevelEnabled(LogLevel level);
};

} // namespace logging
} // namespace networklib

// Convenience Macros
#define LOG_TRACE(fmt, ...) \
    if (networklib::logging::Logger::IsLevelEnabled(networklib::logging::LogLevel::TRACE)) \
        networklib::logging::Logger::Log(networklib::logging::LogLevel::TRACE, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#define LOG_DEBUG(fmt, ...) \
    if (networklib::logging::Logger::IsLevelEnabled(networklib::logging::LogLevel::DEBUG)) \
        networklib::logging::Logger::Log(networklib::logging::LogLevel::DEBUG, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#define LOG_INFO(fmt, ...) \
    if (networklib::logging::Logger::IsLevelEnabled(networklib::logging::LogLevel::INFO)) \
        networklib::logging::Logger::Log(networklib::logging::LogLevel::INFO, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#define LOG_WARN(fmt, ...) \
    if (networklib::logging::Logger::IsLevelEnabled(networklib::logging::LogLevel::WARN)) \
        networklib::logging::Logger::Log(networklib::logging::LogLevel::WARN, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#define LOG_ERROR(fmt, ...) \
    if (networklib::logging::Logger::IsLevelEnabled(networklib::logging::LogLevel::ERROR)) \
        networklib::logging::Logger::Log(networklib::logging::LogLevel::ERROR, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#define LOG_FATAL(fmt, ...) \
    if (networklib::logging::Logger::IsLevelEnabled(networklib::logging::LogLevel::FATAL)) \
        networklib::logging::Logger::Log(networklib::logging::LogLevel::FATAL, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#define LOG_CRITICAL(fmt, ...) \
    if (networklib::logging::Logger::IsLevelEnabled(networklib::logging::LogLevel::CRITICAL)) \
        networklib::logging::Logger::Log(networklib::logging::LogLevel::CRITICAL, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
