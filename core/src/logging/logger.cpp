#include "networklib/logging.h"
#include "async_logger.hpp"
#include "log_manager.hpp"
#include <cstdarg>
#include <cstdio>
#include <thread>
#include <atomic>

#if defined(__linux__) || defined(__APPLE__)
#include <unistd.h>
#include <sys/syscall.h>
#else
#include <processthreadsapi.h>
#endif

namespace networklib {
namespace logging {

// Thread-local sequence number
thread_local unsigned long t_sequence = 0;
// Thread-local thread name (cached)
thread_local std::string t_thread_name;

static std::string GetThreadName() {
    if (t_thread_name.empty()) {
        std::stringstream ss;
        ss << std::this_thread::get_id();
        t_thread_name = ss.str();
    }
    return t_thread_name;
}

static int GetProcessId() {
#if defined(__linux__) || defined(__APPLE__)
    return getpid();
#else
    return GetCurrentProcessId();
#endif
}

} // namespace logging

// Define class methods OUTSIDE namespace blocks to avoid ambiguity if header/source mismatch occurred
bool logging::Logger::Initialize(const char* config_string) {
    auto config = logging::ParseConfig(config_string);
    logging::LogManager::Instance().Initialize(config);
    return true;
}

void logging::Logger::Deinitialize() {
    logging::LogManager::Instance().Shutdown();
}

bool logging::Logger::IsLevelEnabled(logging::LogLevel level) {
    return true;
}

void logging::Logger::Log(logging::LogLevel level, const char* file, int line, const char* func, const char* format, ...) {
    auto logger = logging::LogManager::Instance().GetLogger();
    if (!logger) return;

    logging::LogEntry entry;
    entry.timestamp = std::chrono::system_clock::now();
    entry.level = level;
    entry.file = file ? file : "";
    entry.line = line;
    entry.function = func ? func : "";
    entry.thread_id = std::this_thread::get_id();
    entry.thread_name = logging::GetThreadName();
    entry.process_id = logging::GetProcessId();
    entry.sequence = logging::t_sequence++;

    // Format message
    char buffer[4096];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    entry.message = buffer;

    logger->Enqueue(std::move(entry));
}

void logging::Logger::LogApp(const char* app_name, logging::LogLevel level, const char* file, int line, const char* func, const char* format, ...) {
    auto logger = logging::LogManager::Instance().GetLogger(app_name);
    if (!logger) {
        // Fallback to default if not found?
        // Per requirement "if not provide than use the first one",
        // but this API explicitly asks for one.
        // If not found, let's try default.
        logger = logging::LogManager::Instance().GetLogger();
        if (!logger) return;
    }

    logging::LogEntry entry;
    entry.timestamp = std::chrono::system_clock::now();
    entry.level = level;
    entry.app_name = app_name;
    entry.file = file ? file : "";
    entry.line = line;
    entry.function = func ? func : "";
    entry.thread_id = std::this_thread::get_id();
    entry.thread_name = logging::GetThreadName();
    entry.process_id = logging::GetProcessId();
    entry.sequence = logging::t_sequence++;

    // Format message
    char buffer[4096];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    entry.message = buffer;

    logger->Enqueue(std::move(entry));
}

} // namespace networklib
