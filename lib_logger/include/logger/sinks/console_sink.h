#pragma once

#include "logger/sinks/log_sink.h"
#include "logger/log_common.h"
#include <mutex>

namespace networklib {
namespace logging {

class ConsoleSink : public LogSink {
public:
    explicit ConsoleSink(const LoggerConfig& config);

    void Write(const LogEntry& entry) override;
    void Flush() override;

private:
    LoggerConfig config_;
    std::mutex mutex_;
};

} // namespace logging
} // namespace networklib
