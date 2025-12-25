#ifndef NETWORKLIB_PROTOCOLS_WEBSOCKET_WEBSOCKET_HANDLER_H
#define NETWORKLIB_PROTOCOLS_WEBSOCKET_WEBSOCKET_HANDLER_H

#include "networklib/protocols/protocol_handler.h"
#include "networklib/protocols/websocket/frame.h"
#include "networklib/protocols/http/http_handler.h" // For upgrade detection

namespace networklib {
namespace protocols {
namespace websocket {

class WebSocketHandler : public ProtocolHandler {
public:
    void OnConnection(const core::Connection::Ptr& conn) override;
    void OnMessage(const core::Connection::Ptr& conn) override;

private:
    bool is_upgraded_{false};
    FrameParser parser_;
    // We reuse HttpHandler logic for the handshake phase
    // Or we implement a simple handshake parser here.
    // For clean architecture, we should have a "HandshakeHandler" that switches to "WebSocketHandler".
    // But for Phase 16, we'll embed the logic.
};

} // namespace websocket
} // namespace protocols
} // namespace networklib

#endif // NETWORKLIB_PROTOCOLS_WEBSOCKET_WEBSOCKET_HANDLER_H
