#include "networklib/core/client/client_protocol.h"
#include "networklib/core/client/protocols/tcp_protocol.h"
#include "networklib/core/client/protocols/http_protocol.h"
#include "networklib/core/client/protocols/websocket_protocol.h"
#include "networklib/core/client/protocols/grpc_protocol.h"

namespace networklib {
namespace core {
namespace client {

std::unique_ptr<IClientProtocol> ClientProtocolFactory::Create(const std::string& mode) {
    if (mode == "http") return std::make_unique<HttpClientProtocol>();
    if (mode == "websocket") return std::make_unique<WebSocketClientProtocol>();
    if (mode == "grpc") return std::make_unique<GrpcClientProtocol>();
    return std::make_unique<TcpClientProtocol>();
}

}
}
}
