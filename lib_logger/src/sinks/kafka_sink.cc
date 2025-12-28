#include "logger/sinks/kafka_sink.h"
#include "logger/log_formatter.h"

namespace networklib {
namespace logging {

KafkaSink::KafkaSink(const LoggerConfig& config) : config_(config) {
#ifdef ENABLE_KAFKA
    // RdKafka initialization would go here using config.kafka_broker, config.kafka_topic
#endif
}

void KafkaSink::Write(const LogEntry& entry) {
#ifdef ENABLE_KAFKA
    std::string formatted = LogFormatter::Format(entry, config_);
    // Produce message to Kafka
    // producer_->produce(topic, ..., formatted, ...);
#else
    (void)entry;
#endif
}

void KafkaSink::Flush() {
#ifdef ENABLE_KAFKA
    // Flush producer
#endif
}

} // namespace logging
} // namespace networklib
