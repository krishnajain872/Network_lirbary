#ifndef NETWORKLIB_OBSERVABILITY_LOGGING_LOGGER_H
#define NETWORKLIB_OBSERVABILITY_LOGGING_LOGGER_H

#include <iostream>
#include <string>
#include <mutex>

namespace networklib {
namespace observability {
namespace logging {

// Simple thread-safe logger for Phase 1/2
// In production, this would use spdlog or similar high-perf library
class Logger {
public:
    static Logger& Instance() {
        static Logger instance;
        return instance;
    }

    template<typename T>
    void Info(const T& msg) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << "[INFO] " << msg << std::endl;
    }

    template<typename T>
    void Error(const T& msg) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cerr << "[ERROR] " << msg << std::endl;
    }

private:
    Logger() = default;
    std::mutex mutex_;
};

} // namespace logging
} // namespace observability
} // namespace networklib

#define LOG_INFO(msg) networklib::observability::logging::Logger::Instance().Info(msg)
#define LOG_ERROR(msg) networklib::observability::logging::Logger::Instance().Error(msg)

#endif // NETWORKLIB_OBSERVABILITY_LOGGING_LOGGER_H
