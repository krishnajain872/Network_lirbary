#include "logger/sinks/file_sink.h"
#include "logger/log_formatter.h"
#include <filesystem>

namespace networklib {
namespace logging {

FileSink::FileSink(const LoggerConfig& config)
    : config_(config), path_(config.file_path),
      rotation_size_(config.rotation_size), max_backups_(config.max_backups) {
    Open();
}

FileSink::~FileSink() {
    Close();
}

void FileSink::Write(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (rotation_size_ > 0 && current_size_ >= rotation_size_) {
        Rotate();
    }

    std::string formatted = LogFormatter::Format(entry, config_);
    file_ << formatted << "\n";

    // Approximate size tracking
    current_size_ += formatted.size() + 1;
}

void FileSink::Flush() {
    std::lock_guard<std::mutex> lock(mutex_);
    file_.flush();
}

void FileSink::Open() {
    file_.open(path_, std::ios::app);
    if (file_.is_open()) {
        current_size_ = std::filesystem::file_size(path_);
    }
}

void FileSink::Close() {
    if (file_.is_open()) {
        file_.close();
    }
}

void FileSink::Rotate() {
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

} // namespace logging
} // namespace networklib
