#include "networklib/config/config.h"
#include "networklib/core/event/reactor.h"
#include "networklib/protocols/protocol_handler.h"
#include "networklib/protocols/udp/udp_handler.h"
#include "networklib/core/config/config_hot_reload.h"
#include <iostream>
#include <memory>
#include <csignal>

using namespace networklib;

// Global shutdown hook
std::function<void()> shutdown_handler;
void signal_handler(int) {
    if (shutdown_handler) shutdown_handler();
}

int main(int argc, char** argv) {
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    std::string config_path = "config/examples/basic_tcp.yaml";
    if (argc > 1) {
        config_path = argv[1];
    }

    try {
        std::cout << "Loading configuration from " << config_path << "..." << std::endl;
        auto config = config::ConfigParser::Parse(config_path);

        std::cout << "Starting " << config.name << " (Version: " << config.version << ")" << std::endl;
        std::cout << "Mode: " << config.mode << std::endl;

        auto loop = std::make_unique<core::event::EventLoop>();
        
        core::event::EventLoop::PollerType poller_type = core::event::EventLoop::PollerType::EPOLL;
        if (config.performance.event_loop_type == "io_uring") {
            poller_type = core::event::EventLoop::PollerType::IO_URING;
            std::cout << "Using IO_URING poller" << std::endl;
        } else {
            std::cout << "Using EPOLL poller" << std::endl;
        }

        auto init_res = loop->Init(poller_type);
        if (!init_res) {
            std::cerr << "Failed to init event loop: " << init_res.GetError().Message() << std::endl;
            return 1;
        }
        
        // Create Reactor (manages Loop and TLS)
        // We create it as a shared_ptr alias to capture in lambda easily (or raw ptr)
        // But Reactor owns the loop uniquely. We must keep Reactor alive in main scope.
        core::event::Reactor reactor(std::move(loop));
        
        // Setup initial security config
        core::security::DdosProtection::Config ddos_cfg;
        ddos_cfg.max_conn_per_ip = 1000; // Default or from config
        if (config.resilience.rate_limiter.per_ip_enabled) {
            ddos_cfg.rate_limit_per_ip = config.resilience.rate_limiter.per_ip_requests_per_second;
        }
        reactor.SetDdosConfig(ddos_cfg);

        std::shared_ptr<core::security::TlsContext> tls_ctx = nullptr;
        if (config.ssl.enabled) {
            tls_ctx = std::make_shared<core::security::TlsContext>();
            auto res = tls_ctx->Init(config.ssl.cert_file, config.ssl.key_file);
            if (!res) {
                std::cerr << "Failed to init TLS: " << res.GetError().Message() << std::endl;
                return 1;
            }
        }

        // Setup Hot Reload
        // We capture reactor by reference because main stack frame outlives the hot reload thread
        config::ConfigHotReload hot_reload(config_path, [&](const config::ServerConfig& new_conf) {
            std::cout << "[Main] Config reloaded! Applying changes..." << std::endl;
            
            // 1. Update DDoS Config
            core::security::DdosProtection::Config new_ddos;
            if (new_conf.resilience.rate_limiter.per_ip_enabled) {
                new_ddos.rate_limit_per_ip = new_conf.resilience.rate_limiter.per_ip_requests_per_second;
            }
            reactor.SetDdosConfig(new_ddos);

            // 2. Reload TLS Certificates
            if (new_conf.ssl.enabled && reactor.GetTlsContext()) {
                auto res = reactor.GetTlsContext()->ReloadCertificates(new_conf.ssl.cert_file, new_conf.ssl.key_file);
                if (!res) {
                    std::cerr << "[Main] Failed to reload certs: " << res.GetError().Message() << std::endl;
                }
            }
        });
        hot_reload.Start();

        if (config.mode == "udp") {
            auto handler = std::make_shared<protocols::udp::EchoUdpHandler>();
            auto res = reactor.RegisterUdpServer(config.network.port, handler);
            if (!res) {
                std::cerr << "Failed to register UDP server: " << res.GetError().Message() << std::endl;
                return 1;
            }
        } else {
            auto handler = protocols::ProtocolFactory::Create(config.mode);
            auto res = reactor.RegisterServer(config.network.port, handler, tls_ctx);
            if (!res) {
                std::cerr << "Failed to register server: " << res.GetError().Message() << std::endl;
                return 1;
            }
        }

        shutdown_handler = [&]() {
            std::cout << "Shutting down..." << std::endl;
            hot_reload.Stop();
            reactor.Stop();
        };

        std::cout << "Server running on port " << config.network.port << ". Press Ctrl+C to stop." << std::endl;
        reactor.Run();

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
