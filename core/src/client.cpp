#include "networklib/core/client.h"
#include "stream_envelope.pb.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>

namespace networklib {
namespace core {

Client::Client(const config::ClientConfig& config) : config_(config) {
    loop_ = std::make_unique<event::EventLoop>();
    loop_->Init(); // Should check result

    // Create socket
    int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    connection_ = std::make_shared<Connection>(loop_.get(), fd);
}

Client::~Client() {
    Disconnect();
}

bool Client::Connect() {
    // Start loop thread
    loop_thread_ = std::thread([this]() {
        loop_->Run();
    });

    // Reset promise
    connect_promise_ = std::promise<bool>();
    auto future = connect_promise_.get_future();

    // Connect is async in Connection, but we want to block or wait?
    // Connection::Connect sets state.
    // We need to register to loop.

    auto res = connection_->Connect(config_.network.host, config_.network.port);
    if (!res) return false;

    loop_->AddFd(connection_->Fd(), EPOLLIN | EPOLLOUT | EPOLLET, [this](uint32_t events) {
        if (events & EPOLLIN) connection_->HandleRead();
        if (events & EPOLLOUT) connection_->HandleWrite();
        // Check connection state?
        // Simple client doesn't fully handle async connect callback in this snippet.
    });

    // Naive wait
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return true;
}

void Client::Disconnect() {
    if (connection_) connection_->ForceClose();
    if (loop_) loop_->Stop();
    if (loop_thread_.joinable()) loop_thread_.join();
}

bool Client::Send(const std::string& data) {
    if (!connection_ || !connection_->IsConnected()) return false;
    connection_->Send(data);
    return true;
}

bool Client::Send(const StreamEnvelope& envelope) {
    if (config_.mode == "http") {
        // Convert to HTTP POST
        std::string body;
        if (envelope.has_payload()) body = envelope.payload().data();

        std::string path = "/";
        if (envelope.has_header() && !envelope.header().message_type().empty()) {
            // Parse "GET /path" or just use message_type as path
            path = envelope.header().message_type();
        }

        std::string req = "POST " + path + " HTTP/1.1\r\n";
        req += "Host: " + config_.network.host + "\r\n";
        req += "Content-Length: " + std::to_string(body.size()) + "\r\n";
        req += "\r\n";
        req += body;

        return Send(req);
    } else {
        // TCP / Default: Send serialized proto
        return Send(envelope.SerializeAsString());
    }
}

} // namespace core
} // namespace networklib
