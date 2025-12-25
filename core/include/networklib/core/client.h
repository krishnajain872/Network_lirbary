#ifndef NETWORKLIB_CORE_CLIENT_H
#define NETWORKLIB_CORE_CLIENT_H

#include "networklib/network_lib.h"
#include "networklib/config/config.h"
#include "networklib/core/connection.h"
#include "networklib/core/event/event_loop.h"
#include "networklib/core/client/client_protocol.h"
#include "networklib/security/tls_context.h"
#include <thread>
#include <future>

namespace networklib {
namespace core {

class Client : public IClient {
public:
    Client(const config::ClientConfig& config);
    ~Client() override;

    bool Connect() override;
    void Disconnect() override;
    bool Send(const std::string& data) override;
    bool Send(const StreamEnvelope& envelope) override;
    void RegisterMessageHandler(MessageHandler handler) override;

private:
    config::ClientConfig config_;
    MessageHandler message_handler_;
    std::unique_ptr<client::IClientProtocol> protocol_;
    std::unique_ptr<event::EventLoop> loop_;
    std::shared_ptr<Connection> connection_;
    std::shared_ptr<networklib::security::TlsContext> tls_context_;
    std::thread loop_thread_;
    std::promise<bool> connect_promise_;
};

} // namespace core
} // namespace networklib

#endif // NETWORKLIB_CORE_CLIENT_H
