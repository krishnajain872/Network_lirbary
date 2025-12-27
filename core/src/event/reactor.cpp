#include "networklib/core/event/reactor.h"
#include "networklib/core/event/socket.h"
#include "networklib/core/event/udp_socket.h"
#include "networklib/observability/metrics/metrics_collector.h"
#include "networklib/protocols/udp/udp_handler.h"
#include "networklib/logging.h"
#include <iostream>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace networklib {
namespace core {
namespace event {

Reactor::Reactor(std::unique_ptr<EventLoop> loop) 
    : loop_(std::move(loop)) {
    // Default DDoS config
    ddos_protection_ = std::make_unique<security::DdosProtection>(security::DdosProtection::Config{});
}

void Reactor::SetDdosConfig(const security::DdosProtection::Config& config) {
    ddos_protection_ = std::make_unique<security::DdosProtection>(config);
}

utils::Result<void> Reactor::RegisterServer(int port, 
                                          std::shared_ptr<protocols::ProtocolHandler> handler,
                                          std::shared_ptr<networklib::security::TlsContext> tls_ctx) {
    tls_context_ = tls_ctx; // Store reference for hot reload

    auto sock_res = Socket::CreateNonBlocking();
    if (!sock_res) return utils::Result<void>::Failure(sock_res.GetError().Code(), sock_res.GetError().Message());
    
    int server_fd = sock_res.Value();
    
    auto bind_res = Socket::Bind(server_fd, port);
    if (!bind_res) {
        close(server_fd);
        return bind_res;
    }
    
    auto listen_res = Socket::Listen(server_fd);
    if (!listen_res) {
        close(server_fd);
        return listen_res;
    }

    EventLoop* loop_ptr = loop_.get();

    auto accept_callback = [this, loop_ptr, server_fd, handler](uint32_t events) {
        if (events & EPOLLIN) {
            while (true) {
                // DDoS: Check max connections globally first? 
                // Or rely on Accept logic inside to get IP.
                // We need IP to rate limit per IP.
                
                std::string client_ip;
                
                // We need an Accept variant that returns IP
                // Socket::Accept just returns int.
                // Assuming Socket::Accept logic gets updated or we do it here.
                // For Phase 1, we accept first then check.
                
                struct sockaddr_in addr;
                socklen_t addrlen = sizeof(addr);
                int client_fd = accept4(server_fd, (struct sockaddr*)&addr, &addrlen, SOCK_NONBLOCK);
                
                if (client_fd < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                    break;
                }
                
                char ip_str[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &addr.sin_addr, ip_str, INET_ADDRSTRLEN);
                client_ip = ip_str;

                // DDoS Protection Check
                if (ddos_protection_ && !ddos_protection_->AllowConnection(client_ip)) {
                    logging::Logger::Log(logging::LogLevel::Warn, __FILE__, __LINE__, __FUNCTION__, "[Security] Connection Rejected (DDoS/RateLimit): %s", client_ip.c_str());
                    close(client_fd);
                    continue;
                }

                auto conn = std::make_shared<Connection>(loop_ptr, client_fd);
                conn->SetConnected(); // Fix: Mark accepted connection as connected
                
                if (tls_context_) {
                    SSL* ssl = tls_context_->CreateSsl();
                    if (ssl) {
                        conn->SetSsl(ssl);
                    }
                }

                connections_[client_fd] = conn;

                conn->SetDisconnectCallback([this](const Connection::Ptr& c) {
                     observability::metrics::MetricsCollector::Instance().SetGauge("active_connections", connections_.size() - 1);
                     if (loop_) {
                         loop_->RemoveFd(c->Fd());
                     }
                     connections_.erase(c->Fd());
                });
                
                observability::metrics::MetricsCollector::Instance().SetGauge("active_connections", connections_.size());

                handler->OnConnection(conn);

                loop_ptr->AddFd(client_fd, EPOLLIN | EPOLLET, [conn, handler](uint32_t client_events) {
                    if (client_events & EPOLLIN) {
                        conn->HandleRead();
                        if (conn->IsConnected()) {
                             handler->OnMessage(conn);
                        }
                    }
                    if (client_events & EPOLLOUT) {
                        conn->HandleWrite();
                    }
                    if (client_events & (EPOLLHUP | EPOLLERR)) {
                        conn->HandleClose();
                    }
                });
            }
        }
    };

    return loop_->AddFd(server_fd, EPOLLIN, accept_callback);
}

utils::Result<void> Reactor::RegisterUdpServer(int port, std::shared_ptr<protocols::udp::UdpHandler> handler) {
    auto sock_res = UdpSocket::CreateNonBlocking();
    if (!sock_res) return utils::Result<void>::Failure(sock_res.GetError().Code(), sock_res.GetError().Message());
    
    int fd = sock_res.Value();
    auto bind_res = UdpSocket::Bind(fd, port);
    if (!bind_res) {
        close(fd);
        return bind_res;
    }

    auto udp_callback = [fd, handler](uint32_t events) {
        if (events & EPOLLIN) {
            char buffer[65536];
            while (true) {
                auto res = UdpSocket::RecvFrom(fd, buffer, sizeof(buffer));
                if (!res || res.Value().bytes_read == 0) break;
                
                auto& val = res.Value();
                handler->OnPacket(fd, buffer, val.bytes_read, val.source_ip, val.source_port);
            }
        }
    };

    return loop_->AddFd(fd, EPOLLIN | EPOLLET, udp_callback);
}

void Reactor::Run() {
    loop_->Run();
}

void Reactor::Stop() {
    loop_->Stop();
}

} // namespace event
} // namespace core
} // namespace networklib
