#pragma once

#include "log_sink.hpp"

// Check if Kafka library is available (mock check for now)
#ifdef ENABLE_KAFKA
#include <librdkafka/rdkafkacpp.h>
#endif

namespace networklib {
namespace logging {

class KafkaSink : public LogSink {
public:
    KafkaSink(const std::string& broker, const std::string& topic) {
#ifdef ENABLE_KAFKA
        // RdKafka initialization would go here
#endif
    }

    void Write(const LogEntry& entry) override {
#ifdef ENABLE_KAFKA
        // Produce message to Kafka
#else
        // No-op or throw exception if used without Kafka support
#endif
    }

    void Flush() override {
#ifdef ENABLE_KAFKA
        // Flush producer
#endif
    }
};

} // namespace logging
} // namespace networklib
