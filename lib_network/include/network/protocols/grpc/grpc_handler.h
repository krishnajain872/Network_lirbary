#ifndef NETWORKLIB_PROTOCOLS_GRPC_GRPC_HANDLER_H
#define NETWORKLIB_PROTOCOLS_GRPC_GRPC_HANDLER_H

#include "network/protocols/protocol_handler.h"
#include "network/protocols/http2/session.h"
#include <any>

namespace networklib {
namespace protocols {
namespace grpc {

class GrpcHandler : public ProtocolHandler {
public:
    void OnConnection(const core::Connection::Ptr& conn) override;
    void OnMessage(const core::Connection::Ptr& conn) override;

private:
    // Session state stored in connection context
};

} // namespace grpc
} // namespace protocols
} // namespace networklib

#endif // NETWORKLIB_PROTOCOLS_GRPC_GRPC_HANDLER_H
