#include "logger/log_manager.h"
#include <iostream>

namespace networklib {
namespace logging {

LogManager& LogManager::Instance() {
    static LogManager instance;
    return instance;
}

std::shared_ptr<AsyncLogger> LogManager::Initialize(const LoggerConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);

    // If empty name, default to "default" to ensure we have a key
    std::string name = config.app_name;
    if (name.empty()) {
        name = "default";
    }

    auto it = loggers_.find(name);
    if (it != loggers_.end()) {
        return it->second;
    }

    auto logger = std::make_shared<AsyncLogger>();
    // Make sure we pass the corrected name to the logger if it uses it internally
    LoggerConfig final_config = config;
    final_config.app_name = name;

    logger->Initialize(final_config);
    loggers_[name] = logger;

    if (default_app_name_.empty()) {
        default_app_name_ = name;
    }

    return logger;
}

std::shared_ptr<AsyncLogger> LogManager::GetLogger(const std::string& app_name) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (app_name.empty()) {
        if (default_app_name_.empty()) return nullptr;
         auto it = loggers_.find(default_app_name_);
         if (it != loggers_.end()) return it->second;
         return nullptr;
    }

    auto it = loggers_.find(app_name);
    if (it != loggers_.end()) {
        return it->second;
    }

    return nullptr;
}

void LogManager::Shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& pair : loggers_) {
        pair.second->Shutdown();
    }
    loggers_.clear();
    default_app_name_.clear();
}

LogLevel LogManager::GetDefaultSeverity() const {
    // Return default severity if no logger is initialized
    if (loggers_.empty() && default_app_name_.empty()) return LogLevel::Info;

    // Use default logger if available
    auto it = loggers_.find(default_app_name_);
    if (it != loggers_.end() && it->second) {
        return it->second->GetConfig().severity;
    }
    return LogLevel::Info;
}

LogManager::LogManager() = default;
LogManager::~LogManager() { Shutdown(); }

} // namespace logging
} // namespace networklib
