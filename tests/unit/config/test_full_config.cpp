#include <gtest/gtest.h>
#include "networklib/config/config.h"
#include <fstream>

using namespace networklib::config;

class ConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary config file for testing
        std::ofstream out("test_full_config.yaml");
        out << R"(
server:
  mode: "grpc"
  name: "test-server"
  version: "1.0.0"
  environment: "staging"
  
  network:
    host: "127.0.0.1"
    port: 50051
    protocol: "tcp"
    socket:
      so_reuseaddr: true
      tcp_nodelay: true
      so_rcvbuf: 1048576

  protocols:
    grpc:
      enabled: true
      streaming:
        bidirectional: true
        max_concurrent_streams: 500
      compression:
        enabled: true
        algorithms: ["gzip", "snappy"]
    http:
      enabled: true
      port: 8080
      
  ssl:
    enabled: true
    min_tls_version: "1.3"
    cipher_suites: ["TLS_AES_256_GCM_SHA384"]

  performance:
    max_connections: 5000000
    event_loop_type: "io_uring"
    memory:
      allocator: "jemalloc"
      connection_pool_size: 5000
      
  resilience:
    circuit_breaker:
      enabled: true
      failure_threshold_percentage: 60
    rate_limiter:
      enabled: true
      requests_per_second: 5000

  observability:
    metrics:
      enabled: true
      port: 9091
      metrics_list: ["request_count", "latency"]
    tracing:
      enabled: true
      exporter: "zipkin"
)";
        out.close();
    }

    void TearDown() override {
        remove("test_full_config.yaml");
    }
};

TEST_F(ConfigTest, ParsesFullConfiguration) {
    ServerConfig config = ConfigParser::Parse("test_full_config.yaml");

    // Basic
    EXPECT_EQ(config.mode, "grpc");
    EXPECT_EQ(config.name, "test-server");
    EXPECT_EQ(config.environment, "staging");

    // Network
    EXPECT_EQ(config.network.host, "127.0.0.1");
    EXPECT_EQ(config.network.port, 50051);
    EXPECT_EQ(config.network.socket.so_rcvbuf, 1048576);

    // Protocols
    EXPECT_TRUE(config.protocols.grpc.enabled);
    EXPECT_EQ(config.protocols.grpc.streaming.max_concurrent_streams, 500);
    EXPECT_EQ(config.protocols.grpc.compression.algorithms.size(), 2);
    EXPECT_EQ(config.protocols.grpc.compression.algorithms[1], "snappy");
    
    EXPECT_TRUE(config.protocols.http.enabled);
    EXPECT_EQ(config.protocols.http.port, 8080);

    // SSL
    EXPECT_TRUE(config.ssl.enabled);
    EXPECT_EQ(config.ssl.min_tls_version, "1.3");
    EXPECT_EQ(config.ssl.cipher_suites[0], "TLS_AES_256_GCM_SHA384");

    // Performance
    EXPECT_EQ(config.performance.max_connections, 5000000);
    EXPECT_EQ(config.performance.event_loop_type, "io_uring");
    EXPECT_EQ(config.performance.memory.allocator, "jemalloc");

    // Resilience
    EXPECT_EQ(config.resilience.circuit_breaker.failure_threshold_percentage, 60);
    EXPECT_EQ(config.resilience.rate_limiter.requests_per_second, 5000);

    // Observability
    EXPECT_EQ(config.observability.metrics.port, 9091);
    EXPECT_EQ(config.observability.metrics.metrics_list.size(), 2);
    EXPECT_EQ(config.observability.tracing.exporter, "zipkin");
}
