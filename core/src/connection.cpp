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

    if (state_ == kConnecting) {
        int error = 0;
        socklen_t len = sizeof(error);
        if (getsockopt(fd_, SOL_SOCKET, SO_ERROR, &error, &len) < 0 || error != 0) {
            LOG(Error, "Async connect failed: %s", strerror(error != 0 ? error : errno));
            HandleError();
            return;
        }

        LOG(Info, "Async connect successful for fd %d", fd_);

        if (ssl_) {
            state_ = kHandshaking;
            HandleHandshake();
            return;
        } else {
            state_ = kConnected;
            // Fall through to write any pending data (if any)
        }
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
    loop_->RemoveFd(fd_);
    if (disconnect_callback_) {
        disconnect_callback_(shared_from_this());
    }
}

void Connection::HandleError() {
    LOG(Error, "Connection error on fd %d", fd_);
    HandleClose();
}

void Connection::Send(const char* data, std::size_t len) {
    if (state_ != kConnected) return;
    
    // Thread safety check
    // Assuming loop_ exposes RunInLoop (which I just added)
    // But Connection stores `event::EventLoop* loop_`.
    // We need to cast or access it.
    // EventLoop is defined in `event_loop.h`.

    // Note: IsInLoopThread is const.
    // I need to include <string> to copy data for lambda if queueing.

    // Check thread
    // This requires EventLoop to expose IsInLoopThread
    // I added it.

    // We need to capture data. `std::string` copy is safest.
    // But data is char*.

    /*
       We cannot easily check `loop_->IsInLoopThread()` here without including full definition
       Wait, `event_loop.h` IS included.
    */

    // Note: Since `Send` is called frequently, optimization matters.
    // But correctness first.

    // Using string for capture
    // Check if we are in loop thread
    // Wait, `Send` calls `Write` directly if buffer empty.
    // `Write` calls `SSL_write` or `write`.

    // If not in loop thread, queue it.

    // Issue: Connection::Send signature is `const char*, len`.
    // Queueing requires ownership of data.

    // I will implement a check.
    // But I need to include `<thread>` in `connection.cpp` if I use `std::this_thread` directly,
    // OR rely on `loop_->IsInLoopThread()`.

    // Using `loop_->RunInLoop` is cleaner.

    // Capture by value (string)
    // std::string safe_data(data, len);
    // loop_->RunInLoop([self = shared_from_this(), safe_data]() {
    //     self->SendInternal(safe_data.data(), safe_data.size());
    // });

    // But I can't change signature of Send easily without breaking callers?
    // I'll modify Send body.

    // But wait, RunInLoop takes `void()`.
    // I need to know if I should queue.

    // I'll check `loop_->IsInLoopThread()`.
    // If false:
    //   std::string d(data, len);
    //   loop_->RunInLoop([this, d, len] { Send(d.data(), d.size()); });
    //   return;

    // But `this` might die? `shared_from_this()` is safer.

    // Okay.

    // I need to add `SendInternal`? No, just call `Send` recursively (it will pass check).
    // Or just `Send(const string&)` overload calls `Send(char*, len)`.

    // Let's do it in `Send(const char*, len)`.

    // But `loop_` is raw pointer. `Connection` owns it? No, Reactor owns Loop. Connection has ptr.
    // If Loop dies, Connection dies?
    // Connection holds Loop ptr.

    if (!loop_->IsInLoopThread()) {
        std::string d(data, len);
        auto self = shared_from_this();
        loop_->RunInLoop([self, d]() {
            self->Send(d.data(), d.size());
        });
        return;
    }

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
