#pragma once

#include "logger/sinks/log_sink.h"
#include "logger/log_common.h"
#include <mutex>
#include <string>
#include <fstream>

namespace networklib {
namespace logging {

class FileSink : public LogSink {
public:
    explicit FileSink(const LoggerConfig& config);
    ~FileSink() override;

    void Write(const LogEntry& entry) override;
    void Flush() override;

private:
    void Open();
    void Close();
    void Rotate();

    LoggerConfig config_;
    std::string path_;
    size_t rotation_size_;
    int max_backups_;
    std::ofstream file_;
    size_t current_size_ = 0;
    std::mutex mutex_;
};

} // namespace logging
} // namespace networklib
