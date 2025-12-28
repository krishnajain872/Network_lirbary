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
#include <cstdarg>

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
    static bool Initialize(const char* config_string);
    static void Log(LogLevel level, const char* file, int line, const char* func, const char* format, ...);
    static void LogApp(const char* app_name, LogLevel level, const char* file, int line, const char* func, const char* format, ...);
    static void Deinitialize();
    static bool IsLevelEnabled(LogLevel level);
};

} // namespace logging
} // namespace networklib

#define LOG_TRACE(fmt, ...) \
    do { \
        networklib::logging::Logger::Log( \
            networklib::logging::LogLevel::Trace, \
            __FILE__, __LINE__, __FUNCTION__, \
            fmt, ##__VA_ARGS__); \
    } while (0)

#define LOG_DEBUG(fmt, ...) \
    do { \
        networklib::logging::Logger::Log( \
            networklib::logging::LogLevel::Debug, \
            __FILE__, __LINE__, __FUNCTION__, \
            fmt, ##__VA_ARGS__); \
    } while (0)

#define LOG_INFO(fmt, ...) \
    do { \
        networklib::logging::Logger::Log( \
            networklib::logging::LogLevel::Info, \
            __FILE__, __LINE__, __FUNCTION__, \
            fmt, ##__VA_ARGS__); \
    } while (0)

#define LOG_WARN(fmt, ...) \
    do { \
        networklib::logging::Logger::Log( \
            networklib::logging::LogLevel::Warn, \
            __FILE__, __LINE__, __FUNCTION__, \
            fmt, ##__VA_ARGS__); \
    } while (0)

#define LOG_ERROR(fmt, ...) \
    do { \
        networklib::logging::Logger::Log( \
            networklib::logging::LogLevel::Error, \
            __FILE__, __LINE__, __FUNCTION__, \
            fmt, ##__VA_ARGS__); \
    } while (0)
// Backward compatibility macros if needed by internal code
#define LOG(level, fmt, ...) \
    do { \
        if ((fmt) && \
            networklib::logging::Logger::IsLevelEnabled(networklib::logging::LogLevel::level)) { \
            _Pragma("GCC diagnostic push") \
            _Pragma("GCC diagnostic ignored \"-Wformat-security\"") \
            networklib::logging::Logger::Log(networklib::logging::LogLevel::level, \
                __FILE__, __LINE__, __FUNCTION__, (fmt), ##__VA_ARGS__); \
            _Pragma("GCC diagnostic pop") \
        } \
    } while(0)
