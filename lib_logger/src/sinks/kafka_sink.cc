#include "logger/sinks/kafka_sink.h"
#include "logger/log_formatter.h"
#include <iostream>

#ifdef ENABLE_KAFKA
#include <librdkafka/rdkafka.h>

namespace networklib {
namespace logging {

// Helper callback for delivery reports
static void dr_msg_cb(rd_kafka_t *rk, const rd_kafka_message_t *rkmessage, void *opaque) {
    if (rkmessage->err) {
        // In a real system, we might log this to stderr or a fallback file
        // std::cerr << "Message delivery failed: " << rd_kafka_err2str(rkmessage->err) << std::endl;
    }
}

struct KafkaSink::Impl {
    rd_kafka_t *rk = nullptr;
    rd_kafka_topic_t *rkt = nullptr;
    rd_kafka_conf_t *conf = nullptr;
    rd_kafka_topic_conf_t *topic_conf = nullptr;

    ~Impl() {
        if (rkt) rd_kafka_topic_destroy(rkt);
        if (rk) {
            rd_kafka_flush(rk, 1000);
            rd_kafka_destroy(rk);
        }
    }
};

KafkaSink::KafkaSink(const LoggerConfig& config) : config_(config), impl_(new Impl()) {
    char errstr[512];

    impl_->conf = rd_kafka_conf_new();

    // Set bootstrap servers
    if (rd_kafka_conf_set(impl_->conf, "bootstrap.servers", config.kafka_broker.c_str(),
                         errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) {
        std::cerr << "Failed to set broker: " << errstr << std::endl;
        return;
    }

    // Set delivery report callback
    rd_kafka_conf_set_dr_msg_cb(impl_->conf, dr_msg_cb);

    // Create producer instance
    impl_->rk = rd_kafka_new(RD_KAFKA_PRODUCER, impl_->conf, errstr, sizeof(errstr));
    if (!impl_->rk) {
        std::cerr << "Failed to create new producer: " << errstr << std::endl;
        // conf is freed by rd_kafka_new on failure? documentation says so usually,
        // but if it fails early it might not. To be safe, we let it be.
        // Actually rd_kafka_new takes ownership of conf if success.
        return;
    }
    // impl_->conf is now owned by impl_->rk

    impl_->topic_conf = rd_kafka_topic_conf_new();
    impl_->rkt = rd_kafka_topic_new(impl_->rk, config.kafka_topic.c_str(), impl_->topic_conf);
    if (!impl_->rkt) {
        std::cerr << "Failed to create topic object: " << rd_kafka_err2str(rd_kafka_last_error()) << std::endl;
        // topic_conf is owned by rkt on success
    }
}

KafkaSink::~KafkaSink() {
    delete impl_;
}

void KafkaSink::Write(const LogEntry& entry) {
    if (!impl_ || !impl_->rk || !impl_->rkt) return;

    std::string formatted = LogFormatter::Format(entry, config_);

retry:
    int ret = rd_kafka_produce(
        impl_->rkt,
        config_.kafka_partition == -1 ? RD_KAFKA_PARTITION_UA : config_.kafka_partition,
        RD_KAFKA_MSG_F_COPY,
        const_cast<char*>(formatted.c_str()), formatted.size(),
        NULL, 0, // Key
        NULL // Opaque
    );

    if (ret == -1) {
        if (rd_kafka_last_error() == RD_KAFKA_RESP_ERR__QUEUE_FULL) {
            rd_kafka_poll(impl_->rk, 1000); // Block to clear queue
            goto retry;
        }
        // std::cerr << "Failed to produce to topic: " << rd_kafka_err2str(rd_kafka_last_error()) << std::endl;
    }

    rd_kafka_poll(impl_->rk, 0); // Non-blocking poll to serve callbacks
}

void KafkaSink::Flush() {
    if (impl_ && impl_->rk) {
        rd_kafka_flush(impl_->rk, 1000);
    }
}

} // namespace logging
} // namespace networklib

#else

namespace networklib {
namespace logging {

// Stub implementation when Kafka is not enabled
struct KafkaSink::Impl {};

KafkaSink::KafkaSink(const LoggerConfig& config) : config_(config), impl_(nullptr) {}
KafkaSink::~KafkaSink() { delete impl_; }
void KafkaSink::Write(const LogEntry&) {}
void KafkaSink::Flush() {}

} // namespace logging
} // namespace networklib

#endif
