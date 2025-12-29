#ifndef NETWORKLIB_CONFIG_CONFIG_H
#define NETWORKLIB_CONFIG_CONFIG_H

#include <string>
#include <vector>
#include <map>

namespace networklib {
namespace config {

// ==========================================
// Network Configuration
// ==========================================
struct SocketConfig {
    bool so_reuseaddr = true;
    bool so_reuseport = true;
    bool tcp_nodelay = true;
    bool tcp_keepalive = true;
    int tcp_keepalive_idle_sec = 60;
    int tcp_keepalive_interval_sec = 10;
    int tcp_keepalive_count = 3;
    int so_rcvbuf = 2097152;
    int so_sndbuf = 2097152;
    int backlog = 4096;
};

struct NetworkConfig {
    std::string host = "0.0.0.0";
    int port = 0;
    std::string protocol = "tcp";
    std::string ip_version = "dual";
    SocketConfig socket;
};

// ==========================================
// Protocol Configuration
// ==========================================
struct GrpcStreamingConfig {
    bool bidirectional = true;
    bool server_streaming = true;
    bool client_streaming = true;
    bool unary = true;
    int max_concurrent_streams = 1000;
    int initial_window_size = 65535;
    int max_frame_size = 16384;
    int keepalive_time_ms = 7200000;
    int keepalive_timeout_ms = 20000;
    bool keepalive_permit_without_calls = true;
    int max_connection_idle_ms = 300000;
};

struct CompressionConfig {
    bool enabled = true;
    std::vector<std::string> algorithms;
    std::string default_algorithm;
    int compression_level = 6;
};

struct Http2Config {
    int max_header_list_size = 16384;
    bool enable_push = false;
    int initial_connection_window_size = 1048576;
};

struct GrpcConfig {
    bool enabled = false;
    GrpcStreamingConfig streaming;
    int max_receive_message_size_mb = 100;
    int max_send_message_size_mb = 100;
    int max_metadata_size_kb = 8;
    CompressionConfig compression;
    Http2Config http2;
};

struct HttpConfig {
    bool enabled = false;
    int port = 0;
    std::string version = "1.1";
};

struct WebsocketConfig {
    bool enabled = false;
    int port = 0;
};

struct TcpUdpConfig {
    bool enabled = false;
    int port = 0;
};

struct ProtocolsConfig {
    GrpcConfig grpc;
    HttpConfig http;
    WebsocketConfig websocket;
    TcpUdpConfig tcp;
    TcpUdpConfig udp;
};

// ==========================================
// SSL/TLS Configuration
// ==========================================
struct SslConfig {
    bool enabled = false;
    std::string min_tls_version = "1.3";
    std::string max_tls_version = "1.3";
    std::string cert_file;
    std::string key_file;
    std::string ca_file;
    bool verify_client = false;
    bool client_cert_required = false;
    std::vector<std::string> cipher_suites;
    bool session_tickets = true;
    std::string session_ticket_keys_file;
    int session_cache_size = 20000;
    int session_timeout_sec = 86400;
    std::vector<std::string> alpn_protocols;
};

struct ProtocolConfig {
    std::string name;
    std::string type; // tcp, udp, http, grpc, etc.
    std::string mode = "proto"; // "proto" (default) or "raw"
    int port = 0;
    bool enabled = true;
    SslConfig ssl; // Optional per-protocol SSL
};

// ==========================================
// Performance Configuration
// ==========================================
struct MemoryConfig {
    std::string allocator = "system";
    bool enable_memory_pools = true;
    int connection_pool_size = 10000;
    int buffer_pool_size = 10000;
    bool enable_huge_pages = false;
    int huge_page_size_mb = 2;
    int max_memory_per_connection_kb = 512;
    int max_total_memory_gb = 100;
};

struct CpuConfig {
    bool enable_cpu_affinity = false;
    std::vector<int> isolated_cores;
    bool enable_simd = true;
};

struct PerformanceConfig {
    long long max_connections = 1000000;
    int max_connections_per_ip = 1000;
    int connection_timeout_ms = 5000;
    int request_timeout_ms = 30000;
    
    std::string io_threads = "auto";
    std::string worker_threads = "auto";
    bool thread_affinity = false;
    
    bool enable_numa = false;
    std::string numa_policy = "local";
    
    std::string event_loop_type = "auto";
    
    bool enable_zero_copy = true;
    bool use_sendfile = true;
    bool use_splice = true;
    
    bool use_io_uring = false;
    int io_uring_entries = 2048;
    std::vector<std::string> io_uring_flags;
    
    MemoryConfig memory;
    CpuConfig cpu;
};

// ==========================================
// Resilience Configuration
// ==========================================
struct CircuitBreakerConfig {
    bool enabled = true;
    int failure_threshold_percentage = 50;
    int success_threshold = 10;
    int timeout_ms = 60000;
    int min_requests = 20;
    int rolling_window_ms = 10000;
};

struct RateLimiterConfig {
    bool enabled = true;
    std::string algorithm = "token_bucket";
    int requests_per_second = 10000;
    int burst_size = 1000;
    bool per_ip_enabled = true;
    int per_ip_requests_per_second = 100;
    int per_ip_burst_size = 10;
};

struct BulkheadConfig {
    bool enabled = true;
    int max_concurrent_requests = 1000;
    int queue_size = 100;
    int queue_timeout_ms = 5000;
};

struct HealthCheckConfig {
    bool enabled = true;
    int interval_ms = 10000;
    int timeout_ms = 3000;
    int unhealthy_threshold = 3;
    int healthy_threshold = 2;
};

struct ResilienceConfig {
    CircuitBreakerConfig circuit_breaker;
    RateLimiterConfig rate_limiter;
    BulkheadConfig bulkhead;
    HealthCheckConfig health_check;
    
    struct Timeout {
        int connection_timeout_ms = 5000;
        int request_timeout_ms = 30000;
        int idle_timeout_ms = 300000;
    } timeout;
    
    struct GracefulShutdown {
        bool enabled = true;
        int timeout_ms = 30000;
        bool drain_connections = true;
    } graceful_shutdown;
};

// ==========================================
// Observability Configuration
// ==========================================
struct MetricsConfig {
    bool enabled = true;
    int port = 9090;
    std::string format = "prometheus";
    std::string path = "/metrics";
    int collect_interval_ms = 1000;
    bool enable_histograms = true;
    std::vector<double> histogram_buckets;
    std::vector<std::string> metrics_list;
};

struct LoggingConfig {
    std::string level = "info";
    std::string format = "text";
    std::string output = "stdout";
    std::string file_path;
    bool compress = false;
};

struct TracingConfig {
    bool enabled = false;
    std::string exporter;
    std::string endpoint;
    double sampling_rate = 0.01;
};

struct ProfilingConfig {
    bool enabled = false;
    bool cpu_profile_enabled = false;
    bool memory_profile_enabled = false;
};

struct ObservabilityConfig {
    MetricsConfig metrics;
    LoggingConfig logging;
    TracingConfig tracing;
    ProfilingConfig profiling;
};

// ==========================================
// Main Server Configuration
// ==========================================
struct ServerConfig {
    std::string mode = "tcp";
    std::string name;
    std::string version;
    std::string environment = "production";
    
    NetworkConfig network;
    ProtocolsConfig protocols; // Legacy static structure
    std::vector<ProtocolConfig> protocol_list; // New dynamic structure
    SslConfig ssl;
    PerformanceConfig performance;
    ResilienceConfig resilience;
    ObservabilityConfig observability;
};

struct ClientConfig {
    std::string mode = "tcp";
    struct {
        std::string host = "127.0.0.1";
        int port = 80;
    } network;
    SslConfig ssl;
};

class ConfigParser {
public:
    static ServerConfig Parse(const std::string& filepath);
    static ClientConfig ParseClient(const std::string& filepath);
    static std::string GenerateLoggerConfig(const LoggingConfig& config);
};

} // namespace config
} // namespace networklib

#endif // NETWORKLIB_CONFIG_CONFIG_H
