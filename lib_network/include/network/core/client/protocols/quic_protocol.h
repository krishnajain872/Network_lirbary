#ifndef NETWORKLIB_CORE_CLIENT_PROTOCOLS_QUIC_PROTOCOL_H
#define NETWORKLIB_CORE_CLIENT_PROTOCOLS_QUIC_PROTOCOL_H

#include "network/core/client/client_protocol.h"
#include "stream_envelope.pb.h"
#include <vector>
#include <string>

namespace networklib {
namespace core {
namespace client {

class QuicClientProtocol : public IClientProtocol {
public:
    void OnConnect(const std::shared_ptr<Connection>& conn) override {
        // Mock QUIC handshake logging
        // No actual data sent for handshake in this mock
    }

    void Send(const std::shared_ptr<Connection>& conn, const StreamEnvelope& envelope) override {
        // Mock QUIC Packet: [Type:1][StreamID:8][Payload]
        // Type 0x01 = STREAM
        std::string payload = envelope.SerializeAsString();
        // Fallback: if payload empty (raw mode test sending raw string?), use raw string?
        // Client::Send calls this with envelope.
        // If scenario used Raw mode, Client::Send(string) calls Connection::Send directly.
        // So this is only for "Proto" mode scenarios.

        std::vector<char> packet;
        packet.push_back(0x01); // Type
        // Stream ID 0 (8 bytes)
        for(int i=0; i<8; ++i) packet.push_back(0);

        packet.insert(packet.end(), payload.begin(), payload.end());

        conn->Send(std::string(packet.begin(), packet.end()));
    }

    void OnDataReceived(const std::shared_ptr<Connection>& conn, IClient::MessageHandler handler) override {
        auto& buf = conn->InputBuffer();
        if (buf.ReadableBytes() > 0) {
            std::string data(buf.Peek(), buf.ReadableBytes());
            buf.RetrieveAll();

            // Server Mock response: "QUIC-ACK: " + payload
            // We wrap this in an envelope for the handler
            StreamEnvelope env;
            env.mutable_payload()->set_data(data);
            if (handler) handler(env);
        }
    }
};

}
}
}

#endif
