#ifndef NETWORKLIB_CORE_CONNECTION_H
#define NETWORKLIB_CORE_CONNECTION_H

#include <memory>
#include <functional>
#include <any>
#include <openssl/ssl.h>
#include "networklib/core/memory/buffer.h"
#include "networklib/core/event/event_loop.h"
#include "networklib/core/resilience/token_bucket.h"
#include "networklib/utils/result.h"

namespace networklib {
namespace core {

class Connection : public std::enable_shared_from_this<Connection> {
public:
    using Ptr = std::shared_ptr<Connection>;
    using DisconnectCallback = std::function<void(const Ptr&)>;
    using ConnectCallback = std::function<void(bool)>;

    Connection(event::EventLoop* loop, int fd);
    ~Connection();

    // TLS initialization
    enum class SslMode { kServer, kClient };
    void SetSsl(SSL* ssl, SslMode mode = SslMode::kServer);
    bool IsSsl() const { return ssl_ != nullptr; }

    // Callbacks from Reactor/EventLoop
    void HandleRead();
    void HandleWrite();
    void HandleClose();
    void HandleError();

    // Client API
    utils::Result<void> Connect(const std::string& host, int port);

    // Helper for Reactor
    void SetConnected();

    // API for ProtocolHandlers
    void Send(const char* data, std::size_t len);
    void Send(const std::string& data);
    void Send(memory::Buffer& buffer);

    void Shutdown();
    void ForceClose();

    // Accessors
    memory::Buffer& InputBuffer() { return input_buffer_; }
    memory::Buffer& OutputBuffer() { return output_buffer_; }
    int Fd() const { return fd_; }
    bool IsConnected() const { return state_ == kConnected; }

    // Context for protocol state (e.g., HTTP parser state)
    void SetContext(const std::any& context) { context_ = context; }
    const std::any& GetContext() const { return context_; }
    std::any* GetMutableContext() { return &context_; }

    void SetDisconnectCallback(const DisconnectCallback& cb) { disconnect_callback_ = cb; }

    using MessageCallback = std::function<void(const Ptr&)>;
    void SetMessageCallback(const MessageCallback& cb) { message_callback_ = cb; }
    void SetConnectCallback(const ConnectCallback& cb) { connect_callback_ = cb; }

private:
    enum State { kConnecting, kHandshaking, kConnected, kDisconnecting, kDisconnected };
    
    // Internal I/O helpers
    ssize_t Read(char* buf, size_t len);
    ssize_t Write(const char* buf, size_t len);
    void HandleHandshake();

    event::EventLoop* loop_;
    int fd_;
    State state_;
    
    SSL* ssl_;

    memory::Buffer input_buffer_;
    memory::Buffer output_buffer_;
    
    std::shared_ptr<resilience::TokenBucket> rate_limiter_;

    std::any context_;
    DisconnectCallback disconnect_callback_;
    MessageCallback message_callback_;
    ConnectCallback connect_callback_;
};

} // namespace core
} // namespace networklib

#endif // NETWORKLIB_CORE_CONNECTION_H
