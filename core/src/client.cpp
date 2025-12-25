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
    loop_->Init();

    int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    connection_ = std::make_shared<Connection>(loop_.get(), fd);
}

Client::~Client() {
    Disconnect();
}

bool Client::Connect() {
    loop_thread_ = std::thread([this]() {
        loop_->Run();
    });

    connect_promise_ = std::promise<bool>();
    auto future = connect_promise_.get_future();

    // Initialize Protocol Strategy
    protocol_ = client::ClientProtocolFactory::Create(config_.mode);
    protocol_->OnConnect(connection_);

    // Wire up Message Callback
    connection_->SetMessageCallback([this](const Connection::Ptr& conn) {
        if (protocol_) protocol_->OnDataReceived(conn, message_handler_);
    });

    if (config_.ssl.enabled) {
        tls_context_ = std::make_shared<networklib::security::TlsContext>();
        auto res = tls_context_->Init(config_.ssl.cert_file, config_.ssl.key_file);
        // Warning: Init failing handled?
        SSL* ssl = tls_context_->CreateSsl();
        connection_->SetSsl(ssl, Connection::SslMode::kClient);
    }

    auto res = connection_->Connect(config_.network.host, config_.network.port);
    if (!res) return false;

    loop_->AddFd(connection_->Fd(), EPOLLIN | EPOLLOUT | EPOLLET, [this](uint32_t events) {
        if (events & EPOLLIN) connection_->HandleRead();
        if (events & EPOLLOUT) connection_->HandleWrite();
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return true;
}

void Client::Disconnect() {
    if (connection_) connection_->ForceClose();
    if (loop_) loop_->Stop();
    if (loop_thread_.joinable()) loop_thread_.join();
}

void Client::RegisterMessageHandler(MessageHandler handler) {
    message_handler_ = handler;
}

bool Client::Send(const std::string& data) {
    // Raw send bypassing protocol (or protocol uses it)
    if (!connection_ || !connection_->IsConnected()) return false;
    connection_->Send(data);
    return true;
}

bool Client::Send(const StreamEnvelope& envelope) {
    if (!connection_ || !connection_->IsConnected()) return false;
    if (protocol_) {
        protocol_->Send(connection_, envelope);
        return true;
    }
    return false;
}

} // namespace core
} // namespace networklib
