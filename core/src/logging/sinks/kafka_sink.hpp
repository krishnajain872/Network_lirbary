#pragma once

#include "log_sink.hpp"
#include "../log_formatter.hpp"

// Check if Kafka library is available (mock check for now)
#ifdef ENABLE_KAFKA
#include <librdkafka/rdkafkacpp.h>
#endif

namespace networklib {
namespace logging {

class KafkaSink : public LogSink {
public:
    explicit KafkaSink(const LoggerConfig& config) : config_(config) {
#ifdef ENABLE_KAFKA
        // RdKafka initialization would go here using config.kafka_broker, config.kafka_topic
#endif
    }

    void Write(const LogEntry& entry) override {
#ifdef ENABLE_KAFKA
        std::string formatted = LogFormatter::Format(entry, config_);
        // Produce message to Kafka
        // producer_->produce(topic, ..., formatted, ...);
#else
        (void)entry;
#endif
    }

    void Flush() override {
#ifdef ENABLE_KAFKA
        // Flush producer
#endif
    }

private:
    LoggerConfig config_;
};

} // namespace logging
} // namespace networklib
