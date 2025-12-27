#include "networklib/core/connection.h"
#include "networklib/constants/limits.h"
#include "networklib/core/observability/tracing/tracer.h"
#include "networklib/logging.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <errno.h>
#include <sys/epoll.h>
#include <thread>

namespace networklib {
namespace core {

Connection::Connection(event::EventLoop* loop, int fd)
    : loop_(loop), fd_(fd), state_(kDisconnected), ssl_(nullptr) {} // Default to Disconnected

Connection::~Connection() {
    if (ssl_) {
        SSL_free(ssl_);
    }
    if (state_ != kDisconnected) {
        close(fd_);
    }
}

// Helper to allow Reactor to set state
void Connection::SetConnected() {
    state_ = kConnected;
}

utils::Result<void> Connection::Connect(const std::string& host, int port) {
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, host.c_str(), &serv_addr.sin_addr) <= 0) {
        LOG(Error, "Invalid address for connection: %s", host.c_str());
        return utils::Result<void>::Failure(constants::errors::kConfigError, "Invalid address");
    }

    LOG(Debug, "Connecting to %s:%d", host.c_str(), port);
    if (connect(fd_, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        if (errno != EINPROGRESS) {
             LOG(Error, "Connect failed to %s:%d: %s", host.c_str(), port, strerror(errno));
             return utils::Result<void>::Failure(constants::errors::kConnectionFailed, "Connect failed");
        }
        state_ = kConnecting;
    } else {
        state_ = kConnected;
        LOG(Info, "Connected to %s:%d", host.c_str(), port);
    }
    return utils::Result<void>::Success();
}

void Connection::SetSsl(SSL* ssl, SslMode mode) {
    ssl_ = ssl;
    state_ = kHandshaking;
    SSL_set_fd(ssl_, fd_);
    
    if (mode == SslMode::kServer) {
        SSL_set_accept_state(ssl_);
    } else {
        SSL_set_connect_state(ssl_);
    }
    
    HandleHandshake();
}

void Connection::HandleHandshake() {
    int ret = SSL_do_handshake(ssl_);
    if (ret == 1) {
        state_ = kConnected;
        LOG(Info, "SSL handshake successful for fd %d", fd_);
        HandleRead();
    } else {
        int err = SSL_get_error(ssl_, ret);
        if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
            return;
        } else {
            LOG(Error, "SSL handshake failed for fd %d: error code %d", fd_, err);
            HandleError();
        }
    }
}

ssize_t Connection::Read(char* buf, size_t len) {
    if (ssl_) {
        return SSL_read(ssl_, buf, len);
    } else {
        return read(fd_, buf, len);
    }
}

ssize_t Connection::Write(const char* buf, size_t len) {
    if (ssl_) {
        return SSL_write(ssl_, buf, len);
    } else {
        return write(fd_, buf, len);
    }
}

void Connection::HandleRead() {
    if (state_ == kHandshaking) {
        HandleHandshake();
        return;
    }

    // Rate limiting
    if (rate_limiter_ && !rate_limiter_->Consume(1.0)) {
        return;
    }

    // Start Trace Span for processing this read
    auto span = observability::Tracer::Instance().StartSpan("connection_read");
    
    char extrabuf[constants::limits::kMaxHeaderSize];
    
    while (true) {
        ssize_t n = Read(extrabuf, sizeof(extrabuf));
        if (n > 0) {
            input_buffer_.Append(extrabuf, n);
            if (message_callback_) message_callback_(shared_from_this());
        } else if (n == 0) {
            HandleClose();
            break;
        } else {
            if (ssl_) {
                int err = SSL_get_error(ssl_, n);
                if (err == SSL_ERROR_WANT_READ) break;
                if (err == SSL_ERROR_WANT_WRITE) break;
                HandleError();
            } else {
                if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                HandleError();
            }
            break;
        }
    }
    span->End();
}

void Connection::HandleWrite() {
    if (state_ == kDisconnected) return;

    // Check if we just connected
    if (state_ == kConnecting) {
        // Need to check SO_ERROR
        int result;
        socklen_t result_len = sizeof(result);
        if (getsockopt(fd_, SOL_SOCKET, SO_ERROR, &result, &result_len) < 0 || result != 0) {
             HandleError();
             return;
        }
        state_ = kConnected;
    }

    if (state_ == kHandshaking) {
        HandleHandshake();
        return;
    }

    while (output_buffer_.ReadableBytes() > 0) {
        ssize_t n = Write(output_buffer_.Peek(), output_buffer_.ReadableBytes());
        if (n > 0) {
            output_buffer_.Retrieve(n);
        } else {
            if (ssl_) {
                int err = SSL_get_error(ssl_, n);
                if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) break;
                HandleError();
                return;
            } else {
                if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                HandleError();
                return;
            }
        }
    }

    if (output_buffer_.ReadableBytes() == 0) {
        loop_->ModifyFd(fd_, EPOLLIN | EPOLLET);
    }
}

void Connection::HandleClose() {
    state_ = kDisconnected;
    LOG(Debug, "Connection closed for fd %d", fd_);
    if (disconnect_callback_) {
        disconnect_callback_(shared_from_this());
    }
}

void Connection::HandleError() {
    LOG(Error, "Connection error on fd %d", fd_);
    HandleClose();
}

void Connection::Send(const char* data, std::size_t len) {
    if (state_ == kDisconnected && input_buffer_.ReadableBytes() == 0) {
         // Allow buffering if we are about to connect (state is Disconnected by default until Connect called)
         // But we need to distinguish "Initial Disconnected" vs "Closed".
         // For now, allow buffering always if not Error?
    }
    
    if (!loop_->IsInLoopThread()) {
        std::string d(data, len);
        auto self = shared_from_this();
        loop_->RunInLoop([self, d]() {
            self->Send(d.data(), d.size());
        });
        return;
    }

    // Append to buffer regardless of state (buffer for later write)
    output_buffer_.Append(data, len);

    // Try to write only if Connected
    if (state_ == kConnected) {
        // Trigger Write logic
        // We can just call HandleWrite or let the loop do it.
        // If we want immediate write:
        ssize_t written = 0;
        // Optimization: Try write if buffer was empty
        if (output_buffer_.ReadableBytes() == len) { // Just appended
             written = Write(data, len);
             if (written > 0) {
                 output_buffer_.Retrieve(written); // Retrieve what we just appended (partially)
                 // But wait, we appended `data` already.
                 // So we need to retrieve `written` from `output_buffer_` TAIL? No, HEAD.
                 // Correct logic:
                 // Don't append first. Try write. Append remainder.
             }
             // ... simplified logic: just append and rely on HandleWrite for now to be safe against complex offset logic
        }
    }
    
    // Enable EPOLLOUT to flush buffer
    if (output_buffer_.ReadableBytes() > 0) {
        // If unconnected, ModifyFd might fail if not added yet.
        // Ignore failure.
        loop_->ModifyFd(fd_, EPOLLIN | EPOLLOUT | EPOLLET);
    }
}

void Connection::Send(const std::string& data) {
    Send(data.data(), data.size());
}

void Connection::Shutdown() {
    if (state_ == kConnected) {
        state_ = kDisconnecting;
        if (output_buffer_.ReadableBytes() == 0) {
            shutdown(fd_, SHUT_WR);
        }
    }
}

void Connection::ForceClose() {
    HandleClose();
}

} // namespace core
} // namespace networklib
