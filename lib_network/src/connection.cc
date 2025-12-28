#include "network/core/connection.h"
#include "network/constants/limits.h"
#include "network/core/observability/tracing/tracer.h"
#include "logger/logging.h"
#include <arpa/inet.h>
#include <chrono>
#include <errno.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

namespace networklib {
namespace core {

Connection::Connection(event::EventLoop *loop, int fd)
    : loop_(loop), fd_(fd), state_(kDisconnected), ssl_(nullptr) {
} // Default to Disconnected

Connection::~Connection() {
  if (ssl_) {
    SSL_free(ssl_);
  }
  // Only close if not already closed by HandleClose
  if (fd_ >= 0) {
    close(fd_);
    fd_ = -1;
  }
}

// Helper to allow Reactor to set state
void Connection::SetConnected() { state_ = kConnected; }

utils::Result<void> Connection::Connect(const std::string &host, int port) {
  // #region agent log
  {
    std::ofstream log(".cursor/debug.log", std::ios::app);
    auto t = std::chrono::duration_cast<std::chrono::milliseconds>(
                 std::chrono::system_clock::now().time_since_epoch())
                 .count();
    log << "{\"sessionId\":\"debug-session\",\"runId\":\"run1\","
           "\"hypothesisId\":\"A\",\"location\":\"connection.cpp:34\","
           "\"message\":\"Connection::Connect entry\",\"data\":{\"host\":\""
        << host << "\",\"port\":" << port << ",\"fd\":" << fd_
        << "},\"timestamp\":" << t << "}\n";
  }
  // #endregion

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);

  if (inet_pton(AF_INET, host.c_str(), &serv_addr.sin_addr) <= 0) {
    LOG(Error, "Invalid address for connection: %s", host.c_str());
    return utils::Result<void>::Failure(constants::errors::kConfigError,
                                        "Invalid address");
  }

  LOG(Debug, "Connecting to %s:%d", host.c_str(), port);
  if (connect(fd_, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    if (errno != EINPROGRESS) {
      LOG(Error, "Connect failed to %s:%d: %s", host.c_str(), port,
          strerror(errno));
      // #region agent log
      {
        std::ofstream log(".cursor/debug.log", std::ios::app);
        auto t = std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::system_clock::now().time_since_epoch())
                     .count();
        log << "{\"sessionId\":\"debug-session\",\"runId\":\"run1\","
               "\"hypothesisId\":\"C\",\"location\":\"connection.cpp:48\","
               "\"message\":\"Connect failed immediately\",\"data\":{\"errno\":"
            << errno << ",\"errno_str\":\"" << strerror(errno)
            << "\"},\"timestamp\":" << t << "}\n";
      }
      // #endregion
      return utils::Result<void>::Failure(constants::errors::kConnectionFailed,
                                          "Connect failed");
    }
    state_ = kConnecting;
    // #region agent log
    {
      std::ofstream log(".cursor/debug.log", std::ios::app);
      auto t = std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::system_clock::now().time_since_epoch())
                   .count();
      log << "{\"sessionId\":\"debug-session\",\"runId\":\"run1\","
             "\"hypothesisId\":\"A\",\"location\":\"connection.cpp:50\","
             "\"message\":\"Connect returned EINPROGRESS, "
             "state=kConnecting\",\"data\":{\"state\":\"kConnecting\"},"
             "\"timestamp\":"
          << t << "}\n";
    }
    // #endregion
  } else {
    state_ = kConnected;
    LOG(Info, "Connected to %s:%d", host.c_str(), port);
    // #region agent log
    {
      std::ofstream log(".cursor/debug.log", std::ios::app);
      auto t = std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::system_clock::now().time_since_epoch())
                   .count();
      log << "{\"sessionId\":\"debug-session\",\"runId\":\"run1\","
             "\"hypothesisId\":\"A\",\"location\":\"connection.cpp:53\","
             "\"message\":\"Connect succeeded immediately, "
             "state=kConnected\",\"data\":{\"state\":\"kConnected\"},"
             "\"timestamp\":"
          << t << "}\n";
    }
    // #endregion
  }
  return utils::Result<void>::Success();
}

void Connection::SetSsl(SSL *ssl, SslMode mode) {
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

ssize_t Connection::Read(char *buf, size_t len) {
  if (ssl_) {
    return SSL_read(ssl_, buf, len);
  } else {
    return read(fd_, buf, len);
  }
}

ssize_t Connection::Write(const char *buf, size_t len) {
  if (ssl_) {
    return SSL_write(ssl_, buf, len);
  } else {
    return write(fd_, buf, len);
  }
}

void Connection::HandleRead() {
  if (state_ == kDisconnected) {
    return;
  }

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

  while (state_ != kDisconnected) {
    ssize_t n = Read(extrabuf, sizeof(extrabuf));
    if (n > 0) {
      input_buffer_.Append(extrabuf, n);
      if (message_callback_)
        message_callback_(shared_from_this());
    } else if (n == 0) {
      HandleClose();
      break;
    } else {
      if (ssl_) {
        int err = SSL_get_error(ssl_, n);
        if (err == SSL_ERROR_WANT_READ)
          break;
        if (err == SSL_ERROR_WANT_WRITE)
          break;
        HandleError();
      } else {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
          break;
        HandleError();
      }
      break;
    }
  }
  span->End();
}

void Connection::HandleWrite() {
  if (state_ == kDisconnected)
    return;

  // Check if we just connected
  if (state_ == kConnecting) {
    // #region agent log
    {
      std::ofstream log(".cursor/debug.log", std::ios::app);
      auto t = std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::system_clock::now().time_since_epoch())
                   .count();
      log << "{\"sessionId\":\"debug-session\",\"runId\":\"run1\","
             "\"hypothesisId\":\"B\",\"location\":\"connection.cpp:149\","
             "\"message\":\"HandleWrite: checking connection "
             "completion\",\"data\":{\"state\":\"kConnecting\",\"fd\":"
          << fd_ << "},\"timestamp\":" << t << "}\n";
    }
    // #endregion
    // Need to check SO_ERROR
    int result;
    socklen_t result_len = sizeof(result);
    if (getsockopt(fd_, SOL_SOCKET, SO_ERROR, &result, &result_len) < 0 ||
        result != 0) {
      // #region agent log
      {
        std::ofstream log(".cursor/debug.log", std::ios::app);
        auto t = std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::system_clock::now().time_since_epoch())
                     .count();
        log << "{\"sessionId\":\"debug-session\",\"runId\":\"run1\","
               "\"hypothesisId\":\"C\",\"location\":\"connection.cpp:154\","
               "\"message\":\"SO_ERROR check failed, calling "
               "HandleError\",\"data\":{\"so_error\":"
            << result << "},\"timestamp\":" << t << "}\n";
      }
      // #endregion
      HandleError();
      return;
    }
    state_ = kConnected;
    if (connect_callback_) {
      connect_callback_(true);
    }
    // #region agent log
    {
      std::ofstream log(".cursor/debug.log", std::ios::app);
      auto t = std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::system_clock::now().time_since_epoch())
                   .count();
      log << "{\"sessionId\":\"debug-session\",\"runId\":\"run1\","
             "\"hypothesisId\":\"B\",\"location\":\"connection.cpp:157\","
             "\"message\":\"Connection completed, state set to "
             "kConnected\",\"data\":{\"state\":\"kConnected\"},\"timestamp\":"
          << t << "}\n";
    }
    // #endregion
  }

  if (state_ == kHandshaking) {
    HandleHandshake();
    return;
  }

  while (output_buffer_.ReadableBytes() > 0 && state_ != kDisconnected) {
    ssize_t n = Write(output_buffer_.Peek(), output_buffer_.ReadableBytes());
    if (n > 0) {
      output_buffer_.Retrieve(n);
    } else {
      if (ssl_) {
        int err = SSL_get_error(ssl_, n);
        if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE)
          break;
        HandleError();
        return;
      } else {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
          break;
        HandleError();
        return;
      }
    }
  }

  if (output_buffer_.ReadableBytes() == 0 && state_ != kDisconnected) {
    loop_->ModifyFd(fd_, EPOLLIN | EPOLLET);
  }
}

void Connection::HandleClose() {
  if (state_ == kDisconnected) {
    return; // Already closed, prevent multiple calls
  }

  state_ = kDisconnected;
  LOG(Debug, "Connection closed for fd %d", fd_);

  // Remove fd from event loop before closing socket
  if (loop_) {
    loop_->RemoveFd(fd_);
  }

  // Close the socket
  if (fd_ >= 0) {
    close(fd_);
    fd_ = -1; // Mark as closed to prevent double close
  }

  if (disconnect_callback_) {
    disconnect_callback_(shared_from_this());
  }
}

void Connection::HandleError() {
  if (state_ == kDisconnected) {
    return; // Already closed, prevent multiple calls
  }

  if (state_ == kConnecting && connect_callback_) {
    connect_callback_(false);
  }

  LOG(Error, "Connection error on fd %d", fd_);
  HandleClose();
}

void Connection::Send(const char *data, std::size_t len) {
  if (state_ == kDisconnected && input_buffer_.ReadableBytes() == 0) {
    // Allow buffering if we are about to connect (state is Disconnected by
    // default until Connect called) But we need to distinguish "Initial
    // Disconnected" vs "Closed". For now, allow buffering always if not Error?
  }

  if (!loop_->IsInLoopThread()) {
    std::string d(data, len);
    auto self = shared_from_this();
    loop_->RunInLoop([self, d]() { self->Send(d.data(), d.size()); });
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
        output_buffer_.Retrieve(
            written); // Retrieve what we just appended (partially)
                      // But wait, we appended `data` already.
        // So we need to retrieve `written` from `output_buffer_` TAIL? No,
        // HEAD. Correct logic: Don't append first. Try write. Append remainder.
      }
      // ... simplified logic: just append and rely on HandleWrite for now to be
      // safe against complex offset logic
    }
  }

  // Enable EPOLLOUT to flush buffer
  if (output_buffer_.ReadableBytes() > 0) {
    // If unconnected, ModifyFd might fail if not added yet.
    // Ignore failure.
    loop_->ModifyFd(fd_, EPOLLIN | EPOLLOUT | EPOLLET);
  }
}

void Connection::Send(const std::string &data) {
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

void Connection::ForceClose() { HandleClose(); }

} // namespace core
} // namespace networklib
