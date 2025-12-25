#pragma once

#include "log_sink.hpp"
#include <fstream>
#include <mutex>
#include <filesystem>

namespace networklib {
namespace logging {

class FileSink : public LogSink {
public:
    explicit FileSink(const std::string& path, size_t rotation_size = 0, int max_backups = 0)
        : path_(path), rotation_size_(rotation_size), max_backups_(max_backups) {
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

        // Simple text format
        auto time_t = std::chrono::system_clock::to_time_t(entry.timestamp);
        file_ << "[" << std::put_time(std::localtime(&time_t), "%Y-%m-%dT%H:%M:%S") << "] "
              << entry.message << "\n";

        // Approximate size tracking
        current_size_ += entry.message.size() + 30; // + timestamp overhead
    }

    void Flush() override {
        std::lock_guard<std::mutex> lock(mutex_);
        file_.flush();
    }

private:
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
