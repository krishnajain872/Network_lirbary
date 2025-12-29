#include "network/core/client.h"
#include "logger/logging.h"
#include "stream_envelope.pb.h"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>


namespace networklib {
namespace core {

Client::Client(const config::ClientConfig &config) : config_(config) {
  loop_ = std::make_unique<event::EventLoop>();
  auto res = loop_->Init();
  if (!res) {
    LOG(Fatal, "Failed to init loop in Client: %s",
        res.GetError().Message().c_str());
  }

  int socket_type = SOCK_STREAM;
  if (config.mode == "udp") {
      socket_type = SOCK_DGRAM;
  }

  int fd = socket(AF_INET, socket_type | SOCK_NONBLOCK, 0);
  connection_ = std::make_shared<Connection>(loop_.get(), fd);
  LOG(Info, "Client initialized for %s mode", config.mode.c_str());
}

Client::~Client() { Disconnect(); }

bool Client::Connect() {
  // #region agent log
  {
    std::ofstream log(".cursor/debug.log", std::ios::app);
    auto t = std::chrono::duration_cast<std::chrono::milliseconds>(
                 std::chrono::system_clock::now().time_since_epoch())
                 .count();
    log << "{\"sessionId\":\"debug-session\",\"runId\":\"run1\","
           "\"hypothesisId\":\"A\",\"location\":\"client.cpp:29\",\"message\":"
           "\"Client::Connect entry\",\"data\":{\"host\":\""
        << config_.network.host << "\",\"port\":" << config_.network.port
        << ",\"mode\":\"" << config_.mode << "\"},\"timestamp\":" << t << "}\n";
  }
  // #endregion

  connect_promise_ = std::promise<bool>();
  auto future = connect_promise_.get_future();

  connection_->SetConnectCallback([this](bool success) {
    connect_promise_.set_value(success);
  });

  // Start the event loop thread
  if (!loop_thread_.joinable()) {
    // #region agent log
    {
      std::ofstream log(".cursor/debug.log", std::ios::app);
      auto t = std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::system_clock::now().time_since_epoch())
                   .count();
      log << "{\"sessionId\":\"debug-session\",\"runId\":\"run1\","
             "\"hypothesisId\":\"D\",\"location\":\"client.cpp:67\","
             "\"message\":\"Starting event loop "
             "thread\",\"data\":{},\"timestamp\":"
          << t << "}\n";
    }
    // #endregion
    loop_thread_ = std::thread([this]() { loop_->Run(); });
  }

  // Initialize Protocol Strategy
  protocol_ = client::ClientProtocolFactory::Create(config_.mode);
  protocol_->OnConnect(connection_);

  // Wire up Message Callback
  connection_->SetMessageCallback([this](const Connection::Ptr &conn) {
    if (config_.mode == "raw" && raw_message_handler_) {
        // Raw Mode: consume everything
        auto& buf = conn->InputBuffer();
        if (buf.ReadableBytes() > 0) {
            std::string data(buf.Peek(), buf.ReadableBytes());
            buf.RetrieveAll();
            raw_message_handler_(data);
        }
    } else if (protocol_) {
        protocol_->OnDataReceived(conn, message_handler_);
    }
  });

  if (config_.ssl.enabled) {
    tls_context_ = std::make_shared<networklib::security::TlsContext>();
    auto res = tls_context_->Init(networklib::security::SslMode::kClient,
                                  config_.ssl.cert_file, config_.ssl.key_file);
    if (!res) {
      LOG(Error, "Client TLS init failed: %s",
          res.GetError().Message().c_str());
      return false;
    }
    SSL *ssl = tls_context_->CreateSsl();
    connection_->SetSsl(ssl, Connection::SslMode::kClient);
  }

  // Register FD before Connect to ensure we don't miss events if Connect
  // completes immediately
  loop_->AddFd(connection_->Fd(), EPOLLIN | EPOLLOUT | EPOLLET,
               [this](uint32_t events) {
                 if (events & EPOLLIN)
                   connection_->HandleRead();
                 if (events & EPOLLOUT)
                   connection_->HandleWrite();
                 if (events & (EPOLLERR | EPOLLHUP))
                   connection_->HandleError();
               });

  // #region agent log
  {
    std::ofstream log(".cursor/debug.log", std::ios::app);
    auto t = std::chrono::duration_cast<std::chrono::milliseconds>(
                 std::chrono::system_clock::now().time_since_epoch())
                 .count();
    log << "{\"sessionId\":\"debug-session\",\"runId\":\"run1\","
           "\"hypothesisId\":\"D\",\"location\":\"client.cpp:58\",\"message\":"
           "\"Before connection->Connect, loop_thread "
           "joinable\",\"data\":{\"loop_thread_joinable\":"
        << (loop_thread_.joinable() ? "true" : "false")
        << "},\"timestamp\":" << t << "}\n";
  }
  // #endregion

  auto res = connection_->Connect(config_.network.host, config_.network.port);
  if (!res) {
    LOG(Error, "Client connect failed: %s", res.GetError().Message().c_str());
    // #region agent log
    {
      std::ofstream log(".cursor/debug.log", std::ios::app);
      auto t = std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::system_clock::now().time_since_epoch())
                   .count();
      log << "{\"sessionId\":\"debug-session\",\"runId\":\"run1\","
             "\"hypothesisId\":\"C\",\"location\":\"client.cpp:62\","
             "\"message\":\"Connection::Connect returned "
             "failure\",\"data\":{},\"timestamp\":"
          << t << "}\n";
    }
    // #endregion
    return false;
  }

  return future.get();
}

void Client::Disconnect() {
  if (connection_)
    connection_->ForceClose();
  if (loop_)
    loop_->Stop();
  if (loop_thread_.joinable())
    loop_thread_.join();
}

void Client::RegisterMessageHandler(MessageHandler handler) {
  message_handler_ = handler;
}

void Client::RegisterRawMessageHandler(RawMessageHandler handler) {
  raw_message_handler_ = handler;
}

bool Client::Send(const std::string &data) {
  // Raw send bypassing protocol (or protocol uses it)
  if (!connection_ || !connection_->IsConnected())
    return false;
  connection_->Send(data);
  return true;
}

bool Client::Send(const StreamEnvelope &envelope) {
  if (!connection_ || !connection_->IsConnected())
    return false;
  if (protocol_) {
    protocol_->Send(connection_, envelope);
    return true;
  }
  return false;
}

} // namespace core
} // namespace networklib
