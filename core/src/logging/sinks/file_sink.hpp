#pragma once

#include "log_sink.hpp"
#include "../log_formatter.hpp"
#include <fstream>
#include <mutex>
#include <filesystem>

namespace networklib {
namespace logging {

class FileSink : public LogSink {
public:
    explicit FileSink(const LoggerConfig& config)
        : config_(config), path_(config.file_path),
          rotation_size_(config.rotation_size), max_backups_(config.max_backups) {
        Open();
    }

    ~FileSink() override {
        Close();
    }

    void Write(const LogEntry& entry) override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (rotation_size_ > 0 && current_size_ >= rotation_size_) {
            Rotate();
        }

        std::string formatted = LogFormatter::Format(entry, config_);
        file_ << formatted << "\n";

        // Approximate size tracking
        current_size_ += formatted.size() + 1;
    }

    void Flush() override {
        std::lock_guard<std::mutex> lock(mutex_);
        file_.flush();
    }

private:
    LoggerConfig config_;
    std::string path_;
    size_t rotation_size_;
    int max_backups_;
    std::ofstream file_;
    size_t current_size_ = 0;
    std::mutex mutex_;

    void Open() {
        file_.open(path_, std::ios::app);
        if (file_.is_open()) {
            current_size_ = std::filesystem::file_size(path_);
        }
    }

    void Close() {
        if (file_.is_open()) {
            file_.close();
        }
    }

    void Rotate() {
        Close();
        // Rotation logic (rename files)
        // file.log -> file.log.1, file.log.1 -> file.log.2, etc.
        // For simplicity in this iteration, just rename current to .1 and overwrite
        try {
            std::string backup = path_ + ".1";
            if (std::filesystem::exists(backup)) {
                std::filesystem::remove(backup);
            }
            std::filesystem::rename(path_, backup);
        } catch (...) {
            // best effort
        }
        Open();
    }
};

} // namespace logging
} // namespace networklib
