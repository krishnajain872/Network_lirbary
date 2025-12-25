#include "networklib/config/config.h"
#include <yaml-cpp/yaml.h>
#include <iostream>

namespace networklib {
namespace config {

// Helper to safely get values
template<typename T>
T GetSafe(const YAML::Node& node, const std::string& key, const T& default_val) {
    if (node[key]) {
        return node[key].as<T>();
    }
    return default_val;
}

// Helper for vector<string>
std::vector<std::string> GetStringVector(const YAML::Node& node, const std::string& key) {
    std::vector<std::string> result;
    if (node[key] && node[key].IsSequence()) {
        for (const auto& item : node[key]) {
            result.push_back(item.as<std::string>());
        }
    }
    return result;
}

// Helper for vector<double>
std::vector<double> GetDoubleVector(const YAML::Node& node, const std::string& key) {
    std::vector<double> result;
    if (node[key] && node[key].IsSequence()) {
        for (const auto& item : node[key]) {
            result.push_back(item.as<double>());
        }
    }
    return result;
}

// Helper for vector<int>
std::vector<int> GetIntVector(const YAML::Node& node, const std::string& key) {
    std::vector<int> result;
    if (node[key] && node[key].IsSequence()) {
        for (const auto& item : node[key]) {
            result.push_back(item.as<int>());
        }
    }
    return result;
}

ServerConfig ConfigParser::Parse(const std::string& filepath) {
    ServerConfig config;
    try {
        YAML::Node yaml = YAML::LoadFile(filepath);
        if (!yaml["server"]) {
            return config;
        }
        
        YAML::Node server = yaml["server"];
        
        // Basic Info
        config.mode = GetSafe<std::string>(server, "mode", "tcp");
        config.name = GetSafe<std::string>(server, "name", "");
        config.version = GetSafe<std::string>(server, "version", "");
        config.environment = GetSafe<std::string>(server, "environment", "production");
        
        // Network
        if (server["network"]) {
            YAML::Node net = server["network"];
            config.network.host = GetSafe<std::string>(net, "host", "0.0.0.0");
            config.network.port = GetSafe<int>(net, "port", 0);
            config.network.protocol = GetSafe<std::string>(net, "protocol", "tcp");
            config.network.ip_version = GetSafe<std::string>(net, "ip_version", "dual");
            
            if (net["socket"]) {
                config.network.socket.so_reuseaddr = GetSafe<bool>(net["socket"], "so_reuseaddr", true);
                config.network.socket.so_reuseport = GetSafe<bool>(net["socket"], "so_reuseport", true);
                config.network.socket.tcp_nodelay = GetSafe<bool>(net["socket"], "tcp_nodelay", true);
                config.network.socket.tcp_keepalive = GetSafe<bool>(net["socket"], "tcp_keepalive", true);
                config.network.socket.so_rcvbuf = GetSafe<int>(net["socket"], "so_rcvbuf", 2097152);
                config.network.socket.so_sndbuf = GetSafe<int>(net["socket"], "so_sndbuf", 2097152);
            }
        }
        
        // Protocols
        if (server["protocols"]) {
            YAML::Node protos = server["protocols"];
            
            // gRPC
            if (protos["grpc"]) {
                YAML::Node g = protos["grpc"];
                config.protocols.grpc.enabled = GetSafe<bool>(g, "enabled", false);
                config.protocols.grpc.max_receive_message_size_mb = GetSafe<int>(g, "max_receive_message_size_mb", 100);
                
                if (g["streaming"]) {
                    config.protocols.grpc.streaming.bidirectional = GetSafe<bool>(g["streaming"], "bidirectional", true);
                    config.protocols.grpc.streaming.max_concurrent_streams = GetSafe<int>(g["streaming"], "max_concurrent_streams", 1000);
                    config.protocols.grpc.streaming.initial_window_size = GetSafe<int>(g["streaming"], "initial_window_size", 65535);
                }
                
                if (g["compression"]) {
                    config.protocols.grpc.compression.enabled = GetSafe<bool>(g["compression"], "enabled", true);
                    config.protocols.grpc.compression.default_algorithm = GetSafe<std::string>(g["compression"], "default_algorithm", "gzip");
                    config.protocols.grpc.compression.algorithms = GetStringVector(g["compression"], "algorithms");
                }
            }
            
            // HTTP
            if (protos["http"]) {
                config.protocols.http.enabled = GetSafe<bool>(protos["http"], "enabled", false);
                config.protocols.http.port = GetSafe<int>(protos["http"], "port", 0);
                config.protocols.http.version = GetSafe<std::string>(protos["http"], "version", "1.1");
            }
            
            // WebSocket
            if (protos["websocket"]) {
                config.protocols.websocket.enabled = GetSafe<bool>(protos["websocket"], "enabled", false);
                config.protocols.websocket.port = GetSafe<int>(protos["websocket"], "port", 0);
            }

            // TCP/UDP
            if (protos["tcp"]) {
                config.protocols.tcp.enabled = GetSafe<bool>(protos["tcp"], "enabled", false);
                config.protocols.tcp.port = GetSafe<int>(protos["tcp"], "port", 0);
            }
            if (protos["udp"]) {
                config.protocols.udp.enabled = GetSafe<bool>(protos["udp"], "enabled", false);
                config.protocols.udp.port = GetSafe<int>(protos["udp"], "port", 0);
            }
        }
        
        // SSL
        if (server["ssl"]) {
            YAML::Node ssl = server["ssl"];
            config.ssl.enabled = GetSafe<bool>(ssl, "enabled", false);
            config.ssl.min_tls_version = GetSafe<std::string>(ssl, "min_tls_version", "1.3");
            config.ssl.cert_file = GetSafe<std::string>(ssl, "cert_file", "");
            config.ssl.key_file = GetSafe<std::string>(ssl, "key_file", "");
            config.ssl.ca_file = GetSafe<std::string>(ssl, "ca_file", "");
            config.ssl.cipher_suites = GetStringVector(ssl, "cipher_suites");
            config.ssl.alpn_protocols = GetStringVector(ssl, "alpn_protocols");
        }
        
        // Performance
        if (server["performance"]) {
            YAML::Node perf = server["performance"];
            config.performance.max_connections = GetSafe<long long>(perf, "max_connections", 1000000);
            config.performance.event_loop_type = GetSafe<std::string>(perf, "event_loop_type", "auto");
            config.performance.io_threads = GetSafe<std::string>(perf, "io_threads", "auto");
            
            if (perf["memory"]) {
                config.performance.memory.allocator = GetSafe<std::string>(perf["memory"], "allocator", "system");
                config.performance.memory.connection_pool_size = GetSafe<int>(perf["memory"], "connection_pool_size", 10000);
            }
            
            if (perf["cpu"]) {
                config.performance.cpu.enable_cpu_affinity = GetSafe<bool>(perf["cpu"], "enable_cpu_affinity", false);
            }
        }
        
        // Resilience
        if (server["resilience"]) {
            YAML::Node res = server["resilience"];
            
            if (res["circuit_breaker"]) {
                config.resilience.circuit_breaker.enabled = GetSafe<bool>(res["circuit_breaker"], "enabled", true);
                config.resilience.circuit_breaker.failure_threshold_percentage = GetSafe<int>(res["circuit_breaker"], "failure_threshold_percentage", 50);
            }
            
            if (res["rate_limiter"]) {
                config.resilience.rate_limiter.enabled = GetSafe<bool>(res["rate_limiter"], "enabled", true);
                config.resilience.rate_limiter.requests_per_second = GetSafe<int>(res["rate_limiter"], "requests_per_second", 1000);
            }
        }
        
        // Observability
        if (server["observability"]) {
            YAML::Node obs = server["observability"];
            
            if (obs["metrics"]) {
                config.observability.metrics.enabled = GetSafe<bool>(obs["metrics"], "enabled", true);
                config.observability.metrics.port = GetSafe<int>(obs["metrics"], "port", 9090);
                config.observability.metrics.metrics_list = GetStringVector(obs["metrics"], "metrics_list");
            }
            
            if (obs["tracing"]) {
                config.observability.tracing.enabled = GetSafe<bool>(obs["tracing"], "enabled", false);
                config.observability.tracing.exporter = GetSafe<std::string>(obs["tracing"], "exporter", "jaeger");
            }
        }

    } catch (const YAML::Exception& e) {
        std::cerr << "Error parsing config file: " << e.what() << std::endl;
        throw;
    }
    return config;
}

ClientConfig ConfigParser::ParseClient(const std::string& filepath) {
    ClientConfig config;
    try {
        YAML::Node yaml = YAML::LoadFile(filepath);
        if (yaml["client"]) {
            YAML::Node c = yaml["client"];
            config.mode = GetSafe<std::string>(c, "mode", "tcp");
            if (c["connection"]) { // Matches prompt example "connection" block
                // "target": "server:50051" handling logic needed, or just host/port
                // Simplified for now:
                if (c["connection"]["host"]) config.network.host = c["connection"]["host"].as<std::string>();
                if (c["connection"]["port"]) config.network.port = c["connection"]["port"].as<int>();
            }
            if (c["ssl"]) {
                config.ssl.enabled = GetSafe<bool>(c["ssl"], "enabled", false);
                config.ssl.cert_file = GetSafe<std::string>(c["ssl"], "cert_file", "");
                config.ssl.key_file = GetSafe<std::string>(c["ssl"], "key_file", "");
                config.ssl.ca_file = GetSafe<std::string>(c["ssl"], "ca_file", "");
            }
        }
    } catch (...) {
        // Ignore or log
    }
    return config;
}

} // namespace config
} // namespace networklib
