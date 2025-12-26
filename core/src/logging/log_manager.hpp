#pragma once

#include "async_logger.hpp"
#include <map>
#include <mutex>
#include <string>
#include <memory>
#include <iostream>

namespace networklib {
namespace logging {

class LogManager {
public:
    static LogManager& Instance() {
        static LogManager instance;
        return instance;
    }

    // Initialize or get an existing logger for an app
    // Returns weak_ptr? Or shared_ptr. The Manager owns them.
    std::shared_ptr<AsyncLogger> Initialize(const LoggerConfig& config) {
        std::lock_guard<std::mutex> lock(mutex_);

        // If it exists, we might want to update it or just return it.
        // For simplicity, if it exists, we return it. (Or we could re-config).
        auto it = loggers_.find(config.app_name);
        if (it != loggers_.end()) {
            // Ideally re-configure if needed, but for now just return
            return it->second;
        }

        auto logger = std::make_shared<AsyncLogger>();
        logger->Initialize(config);
        loggers_[config.app_name] = logger;

        // If this is the first one, set as default
        if (default_app_name_.empty()) {
            default_app_name_ = config.app_name;
        }

        return logger;
    }

    std::shared_ptr<AsyncLogger> GetLogger(const std::string& app_name = "") {
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

        // Fallback to default if specific not found?
        // Or return default if specific request fails?
        // User requirement: "if not provide than use the first one" (referring to macro usage mostly)
        // If explictly requested "AppB" and it doesn't exist, returning "AppA" might be confusing.
        // But let's return nullptr if explicit request fails.
        return nullptr;
    }

    // Shutdown all
    void Shutdown() {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& pair : loggers_) {
            pair.second->Shutdown();
        }
        loggers_.clear();
        default_app_name_.clear();
    }

private:
    LogManager() = default;
    ~LogManager() { Shutdown(); }

    std::map<std::string, std::shared_ptr<AsyncLogger>> loggers_;
    std::string default_app_name_;
    std::mutex mutex_;
};

} // namespace logging
} // namespace networklib
