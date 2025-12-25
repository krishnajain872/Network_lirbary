#include "networklib/core/connection.h"
#include "networklib/constants/limits.h"
#include "networklib/core/observability/tracing/tracer.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <errno.h>

namespace networklib {
namespace core {

Connection::Connection(event::EventLoop* loop, int fd)
    : loop_(loop), fd_(fd), state_(kConnected), ssl_(nullptr) {}

Connection::~Connection() {
    if (ssl_) {
        SSL_free(ssl_);
    }
    if (state_ != kDisconnected) {
        close(fd_);
    }
}

utils::Result<void> Connection::Connect(const std::string& host, int port) {
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, host.c_str(), &serv_addr.sin_addr) <= 0) {
        return utils::Result<void>::Failure(constants::errors::kConfigError, "Invalid address");
    }

    if (connect(fd_, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        if (errno != EINPROGRESS) {
             return utils::Result<void>::Failure(constants::errors::kConnectionFailed, "Connect failed");
        }
        state_ = kConnecting;
    } else {
        state_ = kConnected;
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
        HandleRead();
    } else {
        int err = SSL_get_error(ssl_, ret);
        if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
            return;
        } else {
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
    if (disconnect_callback_) {
        disconnect_callback_(shared_from_this());
    }
}

void Connection::HandleError() {
    HandleClose();
}

void Connection::Send(const char* data, std::size_t len) {
    if (state_ != kConnected) return;
    
    ssize_t written = 0;
    if (output_buffer_.ReadableBytes() == 0) {
        written = Write(data, len);
        if (written > 0) {
            if (static_cast<std::size_t>(written) == len) return;
        } else {
            if (ssl_) {
                 int err = SSL_get_error(ssl_, written);
                 if (err != SSL_ERROR_WANT_WRITE && err != SSL_ERROR_WANT_READ) {
                     HandleError();
                     return;
                 }
                 written = 0;
            } else {
                if (written < 0) {
                    written = 0;
                    if (errno != EAGAIN && errno != EWOULDBLOCK) {
                        HandleError();
                        return;
                    }
                }
            }
        }
    }
    
    output_buffer_.Append(data + written, len - written);
    
    if (output_buffer_.ReadableBytes() > 0) {
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
