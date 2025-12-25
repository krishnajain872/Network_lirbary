#ifndef NETWORKLIB_CORE_CLIENT_CLIENT_PROTOCOL_H
#define NETWORKLIB_CORE_CLIENT_CLIENT_PROTOCOL_H

#include "networklib/core/connection.h"
#include "networklib/network_lib.h" // For StreamEnvelope, MessageHandler
#include <memory>

namespace networklib {
namespace core {
namespace client {

class IClientProtocol {
public:
    virtual ~IClientProtocol() = default;

    /**
     * @brief Called when connection is established.
     * Use this to send initial handshakes (e.g. HTTP/2 Preface, WS Upgrade).
     */
    virtual void OnConnect(const std::shared_ptr<Connection>& conn) = 0;

    /**
     * @brief Send a StreamEnvelope.
     */
    virtual void Send(const std::shared_ptr<Connection>& conn, const StreamEnvelope& envelope) = 0;

    /**
     * @brief Handle incoming data.
     * The protocol should parse the buffer in conn, extract messages, and call the handler.
     */
    virtual void OnDataReceived(const std::shared_ptr<Connection>& conn, IClient::MessageHandler handler) = 0;
};

class ClientProtocolFactory {
public:
    static std::unique_ptr<IClientProtocol> Create(const std::string& mode);
};

} // namespace client
} // namespace core
} // namespace networklib

#endif // NETWORKLIB_CORE_CLIENT_CLIENT_PROTOCOL_H
