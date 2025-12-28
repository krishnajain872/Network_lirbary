#pragma once

#include "logger/sinks/log_sink.h"
#include "logger/log_common.h"

namespace networklib {
namespace logging {

class KafkaSink : public LogSink {
public:
    explicit KafkaSink(const LoggerConfig& config);
    ~KafkaSink() override;

    void Write(const LogEntry& entry) override;
    void Flush() override;

private:
    LoggerConfig config_;
    struct Impl;
    Impl* impl_;
};

} // namespace logging
} // namespace networklib
