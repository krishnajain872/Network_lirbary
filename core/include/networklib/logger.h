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
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Fatal,
    Critical
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
    if (networklib::logging::Logger::IsLevelEnabled(networklib::logging::LogLevel::Trace)) \
        networklib::logging::Logger::Log(networklib::logging::LogLevel::Trace, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#define LOG_DEBUG(fmt, ...) \
    if (networklib::logging::Logger::IsLevelEnabled(networklib::logging::LogLevel::Debug)) \
        networklib::logging::Logger::Log(networklib::logging::LogLevel::Debug, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#define LOG_INFO(fmt, ...) \
    if (networklib::logging::Logger::IsLevelEnabled(networklib::logging::LogLevel::Info)) \
        networklib::logging::Logger::Log(networklib::logging::LogLevel::Info, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#define LOG_WARN(fmt, ...) \
    if (networklib::logging::Logger::IsLevelEnabled(networklib::logging::LogLevel::Warn)) \
        networklib::logging::Logger::Log(networklib::logging::LogLevel::Warn, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#define LOG_ERROR(fmt, ...) \
    if (networklib::logging::Logger::IsLevelEnabled(networklib::logging::LogLevel::Error)) \
        networklib::logging::Logger::Log(networklib::logging::LogLevel::Error, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#define LOG_FATAL(fmt, ...) \
    if (networklib::logging::Logger::IsLevelEnabled(networklib::logging::LogLevel::Fatal)) \
        networklib::logging::Logger::Log(networklib::logging::LogLevel::Fatal, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#define LOG_CRITICAL(fmt, ...) \
    if (networklib::logging::Logger::IsLevelEnabled(networklib::logging::LogLevel::Critical)) \
        networklib::logging::Logger::Log(networklib::logging::LogLevel::Critical, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
