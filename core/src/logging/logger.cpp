#include "networklib/logger.h"
#include "async_logger.hpp"
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

// Singleton instance
static AsyncLogger* g_logger = nullptr;
static LoggerConfig g_config;

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
    if (logging::g_logger) return false; // Already initialized

    logging::g_config = logging::ParseConfig(config_string);
    logging::g_logger = new logging::AsyncLogger();
    logging::g_logger->Initialize(logging::g_config);
    return true;
}

void logging::Logger::Deinitialize() {
    if (logging::g_logger) {
        logging::g_logger->Shutdown();
        delete logging::g_logger;
        logging::g_logger = nullptr;
    }
}

bool logging::Logger::IsLevelEnabled(logging::LogLevel level) {
    if (!logging::g_logger) return false;
    return level >= logging::g_config.severity;
}

void logging::Logger::Log(logging::LogLevel level, const char* file, int line, const char* func, const char* format, ...) {
    if (!logging::g_logger || !IsLevelEnabled(level)) return;

    logging::LogEntry entry;
    entry.timestamp = std::chrono::system_clock::now();
    entry.level = level;
    entry.file = file ? file : "";
    entry.line = line;
    entry.function = func ? func : "";
    entry.thread_id = std::this_thread::get_id();
    entry.thread_name = logging::GetThreadName(); // Uses thread_local cache
    entry.process_id = logging::GetProcessId();
    entry.sequence = logging::t_sequence++;

    // Format message
    char buffer[4096];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    entry.message = buffer;

    logging::g_logger->Enqueue(std::move(entry));
}

} // namespace networklib
