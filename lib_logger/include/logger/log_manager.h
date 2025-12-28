#pragma once

#include <map>
#include <mutex>
#include <string>
#include <memory>
#include "logger/async_logger.h"

namespace networklib {
namespace logging {

class LogManager {
public:
    static LogManager& Instance();

    std::shared_ptr<AsyncLogger> Initialize(const LoggerConfig& config);
    std::shared_ptr<AsyncLogger> GetLogger(const std::string& app_name = "");
    void Shutdown();

private:
    LogManager();
    ~LogManager();

    std::map<std::string, std::shared_ptr<AsyncLogger>> loggers_;
    std::string default_app_name_;
    std::mutex mutex_;
};

} // namespace logging
} // namespace networklib
