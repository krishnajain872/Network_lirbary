#ifndef NETWORKLIB_CORE_CLIENT_PROTOCOLS_WEBSOCKET_PROTOCOL_H
#define NETWORKLIB_CORE_CLIENT_PROTOCOLS_WEBSOCKET_PROTOCOL_H

#include "networklib/core/client/client_protocol.h"
#include "stream_envelope.pb.h"
#include <vector>

namespace networklib {
namespace core {
namespace client {

class WebSocketClientProtocol : public IClientProtocol {
public:
    void OnConnect(const std::shared_ptr<Connection>& conn) override {
        // Send Upgrade Request
        std::stringstream ss;
        ss << "GET / HTTP/1.1\r\n";
        ss << "Host: localhost\r\n";
        ss << "Upgrade: websocket\r\n";
        ss << "Connection: Upgrade\r\n";
        ss << "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n";
        ss << "Sec-WebSocket-Version: 13\r\n";
        ss << "\r\n";
        conn->Send(ss.str());
    }

    void Send(const std::shared_ptr<Connection>& conn, const StreamEnvelope& envelope) override {
        // Simple Text Frame
        std::string payload;
        if (envelope.has_payload()) payload = envelope.payload().data();

        std::vector<char> out;
        out.push_back(0x81); // Fin | Text
        out.push_back(0x80 | (payload.size() & 0x7F)); // Masked bit set
        // TODO: Proper length encoding > 125

        // Masking Key (0 for now, but valid WS requires 4 bytes)
        out.push_back(0); out.push_back(0); out.push_back(0); out.push_back(0);

        // Mask payload (XOR with 0 = Identity)
        out.insert(out.end(), payload.begin(), payload.end());
        conn->Send(std::string(out.begin(), out.end()));
    }

    void OnDataReceived(const std::shared_ptr<Connection>& conn, IClient::MessageHandler handler) override {
        // Reuse FrameParser? It's in `core/src/protocols/websocket/frame_parser.h`.
        // Including it requires path.
        // For simplicity, naive parsing again or fallback.
        // If upgraded, parse frames.
        // If not upgraded, check response 101.
        auto& buf = conn->InputBuffer();
        if (!upgraded_) {
            if (strstr(buf.Peek(), "101 Switching Protocols")) {
                upgraded_ = true;
                buf.RetrieveAll(); // Clear buffer
            }
            return;
        }

        // Parse Frame (Simplified)
        if (buf.ReadableBytes() >= 2) {
            // uint8_t b1 = buf.Peek()[0];
            uint8_t b2 = buf.Peek()[1];
            size_t len = b2 & 0x7F;
            size_t header_len = 2;
            if (len == 126) header_len = 4; // Not implemented

            if (buf.ReadableBytes() >= header_len + len) {
                std::string msg(buf.Peek() + header_len, len);

                StreamEnvelope env;
                env.mutable_payload()->set_data(msg);
                env.mutable_header()->set_message_type("websocket.message");
                if (handler) handler(env);

                buf.Retrieve(header_len + len);
            }
        }
    }

private:
    bool upgraded_ = false;
};

}
}
}

#endif
