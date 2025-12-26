#include "networklib/core/server.h"
#include "networklib/protocols/protocol_handler.h"
#include "networklib/protocols/udp/udp_handler.h"
#include "networklib/core/event/event_loop.h"
#include "networklib/logger.h"
#include <iostream>

namespace networklib {
namespace core {

Server::Server(const config::ServerConfig& config) : config_(config) {}

Server::~Server() {
    Stop();
}

void Server::Init() {
    auto loop = std::make_unique<event::EventLoop>();

    event::EventLoop::PollerType poller_type = event::EventLoop::PollerType::EPOLL;
    if (config_.performance.event_loop_type == "io_uring") {
        poller_type = event::EventLoop::PollerType::IO_URING;
    }

    auto init_res = loop->Init(poller_type);
    if (!init_res) {
        throw std::runtime_error("Failed to init event loop: " + init_res.GetError().Message());
    }

    reactor_ = std::make_unique<event::Reactor>(std::move(loop));

    // DDoS Protection
    networklib::core::security::DdosProtection::Config ddos_cfg;
    ddos_cfg.max_conn_per_ip = 1000;
    if (config_.resilience.rate_limiter.per_ip_enabled) {
        ddos_cfg.rate_limit_per_ip = config_.resilience.rate_limiter.per_ip_requests_per_second;
    }
    reactor_->SetDdosConfig(ddos_cfg);

    // TLS
    std::shared_ptr<networklib::security::TlsContext> tls_ctx = nullptr;
    if (config_.ssl.enabled) {
        tls_ctx = std::make_shared<networklib::security::TlsContext>();
        auto res = tls_ctx->Init(config_.ssl.cert_file, config_.ssl.key_file);
        if (!res) {
            throw std::runtime_error("Failed to init TLS: " + res.GetError().Message());
        }
    }

    // Register Handlers
    std::shared_ptr<protocols::ProtocolHandler> handler;
    if (config_.mode == "udp") {
        auto udp_handler = std::make_shared<protocols::udp::EchoUdpHandler>();
        if (stream_handler_) {
            udp_handler->SetStreamHandler(stream_handler_);
        }
        auto res = reactor_->RegisterUdpServer(config_.network.port, udp_handler);
        if (!res) throw std::runtime_error(res.GetError().Message());
    } else {
        handler = protocols::ProtocolFactory::Create(config_.mode);
        if (stream_handler_) {
            handler->SetStreamHandler(stream_handler_);
        }

        // Resilience
        if (config_.resilience.rate_limiter.enabled) {
            auto bucket = std::make_shared<resilience::TokenBucket>(
                config_.resilience.rate_limiter.requests_per_second,
                config_.resilience.rate_limiter.burst_size
            );
            handler->SetRateLimiter(bucket);
        }

        auto res = reactor_->RegisterServer(config_.network.port, handler, tls_ctx);
        if (!res) throw std::runtime_error(res.GetError().Message());
    }
    LOG(Info, "Server initialized: %s mode on port %d", config_.mode.c_str(), config_.network.port);
}

void Server::RegisterStreamHandler(StreamHandler handler) {
    stream_handler_ = handler;
}

bool Server::Start() {
    if (running_) return true;

    try {
        Init();

        // Setup Hot Reload
        // Note: passing empty path for now as we constructed from Config object
        // If we want hot reload, we need the path.
        // For simplicity in this step, skipping hot reload if path unknown,
        // or we need to store the path in ServerConfig or Server class.
        // Assuming no hot reload if constructed directly for now.

        running_ = true;
        runner_thread_ = std::thread([this]() {
            reactor_->Run();
        });
        return true;
    } catch (const std::exception& e) {
        LOG(Error, "Server start failed: %s", e.what());
        return false;
    }
}

void Server::Stop() {
    if (!running_) return;
    running_ = false;
    if (reactor_) reactor_->Stop();
    if (runner_thread_.joinable()) runner_thread_.join();
}

void Server::Wait() {
    if (runner_thread_.joinable()) runner_thread_.join();
}

void Server::ReloadConfig(const config::ServerConfig& new_conf) {
    if (!reactor_) return;

    security::DdosProtection::Config new_ddos;
    if (new_conf.resilience.rate_limiter.per_ip_enabled) {
        new_ddos.rate_limit_per_ip = new_conf.resilience.rate_limiter.per_ip_requests_per_second;
    }
    reactor_->SetDdosConfig(new_ddos);

    if (new_conf.ssl.enabled && reactor_->GetTlsContext()) {
        reactor_->GetTlsContext()->ReloadCertificates(new_conf.ssl.cert_file, new_conf.ssl.key_file);
    }
}

} // namespace core
} // namespace networklib
