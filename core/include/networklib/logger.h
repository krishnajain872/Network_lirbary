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

    // Main logging function - uses default logger
    static void Log(LogLevel level, const char* file, int line, const char* func, const char* format, ...);

    // Log with explicit app name
    static void LogApp(const char* app_name, LogLevel level, const char* file, int line, const char* func, const char* format, ...);

    // Cleanup and flush
    static void Deinitialize();

    // Check if a level is enabled (for fast skipping)
    static bool IsLevelEnabled(LogLevel level);
};

} // namespace logging
} // namespace networklib

// Convenience Macros
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

#define LOG_APP(app_name, level, fmt, ...) \
    do { \
        if ((fmt)) { \
            _Pragma("GCC diagnostic push") \
            _Pragma("GCC diagnostic ignored \"-Wformat-security\"") \
            networklib::logging::Logger::LogApp((app_name), networklib::logging::LogLevel::level, \
                __FILE__, __LINE__, __FUNCTION__, (fmt), ##__VA_ARGS__); \
            _Pragma("GCC diagnostic pop") \
        } \
    } while(0)
