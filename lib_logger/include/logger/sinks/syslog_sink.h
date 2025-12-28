#pragma once

#include "logger/sinks/log_sink.h"
#include "logger/log_common.h"

namespace networklib {
namespace logging {

class SyslogSink : public LogSink {
public:
    explicit SyslogSink(const LoggerConfig& config);
    ~SyslogSink() override;

    void Write(const LogEntry& entry) override;
    void Flush() override;

private:
    LoggerConfig config_;
};

} // namespace logging
} // namespace networklib
