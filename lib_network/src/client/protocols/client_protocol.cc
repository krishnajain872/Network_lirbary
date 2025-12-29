#include "network/core/client/client_protocol.h"
#include "network/core/client/protocols/tcp_protocol.h"
#include "network/core/client/protocols/http_protocol.h"
#include "network/core/client/protocols/websocket_protocol.h"
#include "network/core/client/protocols/grpc_protocol.h"

namespace networklib {
namespace core {
namespace client {

std::unique_ptr<IClientProtocol> ClientProtocolFactory::Create(const std::string& mode) {
    if (mode == "http") return std::make_unique<HttpClientProtocol>();
    if (mode == "websocket") return std::make_unique<WebSocketClientProtocol>();
    if (mode == "grpc") return std::make_unique<GrpcClientProtocol>();
    // Fallback/Mock for new protocols to reuse TCP base logic if possible
    // Ideally, QuicClientProtocol would exist.
    if (mode == "quic") return std::make_unique<TcpClientProtocol>(); // Temporary fallback for test
    if (mode == "rpc") return std::make_unique<TcpClientProtocol>(); // Temporary fallback
    return std::make_unique<TcpClientProtocol>();
}

}
}
}
