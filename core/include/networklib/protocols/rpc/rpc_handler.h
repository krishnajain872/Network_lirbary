#ifndef NETWORKLIB_PROTOCOLS_RPC_RPC_HANDLER_H
#define NETWORKLIB_PROTOCOLS_RPC_RPC_HANDLER_H

#include "networklib/protocols/protocol_handler.h"
#include "networklib/core/connection.h"

namespace networklib {
namespace protocols {
namespace rpc {

/**
 * @brief Handles custom RPCs using StreamEnvelope.
 * 
 * Logic:
 * 1. Read 4-byte length prefix.
 * 2. Read Protobuf data.
 * 3. Deserialize StreamEnvelope.
 * 4. Dispatch based on message_type.
 */
class RpcHandler : public ProtocolHandler {
public:
    void OnConnection(const core::Connection::Ptr& conn) override;
    void OnMessage(const core::Connection::Ptr& conn) override;

private:
    bool ParseMessage(core::memory::Buffer& buf);
};

} // namespace rpc
} // namespace protocols
} // namespace networklib

#endif // NETWORKLIB_PROTOCOLS_RPC_RPC_HANDLER_H
