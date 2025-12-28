#include "network/protocols/protocol_handler.h"
#include "network/protocols/tcp/tcp_handler.h"
#include "network/protocols/http/http_handler.h"
#include "network/protocols/grpc/grpc_handler.h"
#include "network/protocols/websocket/websocket_handler.h"
#include "network/protocols/rpc/rpc_handler.h"
#include "network/protocols/udp/udp_handler.h"
#include "network/protocols/quic/quic_handler.h"
#include <stdexcept>

namespace networklib {
namespace protocols {

std::shared_ptr<ProtocolHandler> ProtocolFactory::Create(const std::string& mode) {
    if (mode == "tcp") {
        return std::make_shared<tcp::TcpHandler>();
    } else if (mode == "http") {
        return std::make_shared<http::HttpHandler>();
    } else if (mode == "grpc") {
        return std::make_shared<grpc::GrpcHandler>();
    } else if (mode == "websocket") {
        return std::make_shared<websocket::WebSocketHandler>();
    } else if (mode == "rpc") {
        return std::make_shared<rpc::RpcHandler>();
    } else if (mode == "quic") {
        return std::make_shared<quic::QuicHandler>();
    } else if (mode == "udp") {
        // UDP needs special handling in Main, but Factory can return nullptr or throw specific error
        throw std::runtime_error("UDP is not a stream protocol. Use CreateUdpHandler.");
    }
    throw std::runtime_error("Unknown protocol mode: " + mode);
}

} // namespace protocols
} // namespace networklib
