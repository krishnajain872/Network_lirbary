#pragma once

#include "logger/sinks/log_sink.h"
#include "logger/log_common.h"

#ifdef ENABLE_KAFKA
#include <librdkafka/rdkafkacpp.h>
#endif

namespace networklib {
namespace logging {

class KafkaSink : public LogSink {
public:
    explicit KafkaSink(const LoggerConfig& config);

    void Write(const LogEntry& entry) override;
    void Flush() override;

private:
    LoggerConfig config_;
};

} // namespace logging
} // namespace networklib
