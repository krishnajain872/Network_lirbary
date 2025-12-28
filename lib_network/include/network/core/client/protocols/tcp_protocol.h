#ifndef NETWORKLIB_CORE_CLIENT_PROTOCOLS_TCP_PROTOCOL_H
#define NETWORKLIB_CORE_CLIENT_PROTOCOLS_TCP_PROTOCOL_H

#include "network/core/client/client_protocol.h"
#include "stream_envelope.pb.h"
#include <arpa/inet.h>

namespace networklib {
namespace core {
namespace client {

class TcpClientProtocol : public IClientProtocol {
public:
    void OnConnect(const std::shared_ptr<Connection>& /*conn*/) override {}

    void Send(const std::shared_ptr<Connection>& conn, const StreamEnvelope& envelope) override {
        std::string payload = envelope.SerializeAsString();
        uint32_t length = htonl(static_cast<uint32_t>(payload.size()));
        std::string frame;
        frame.append(reinterpret_cast<const char*>(&length), sizeof(length));
        frame.append(payload);
        conn->Send(frame);
    }

    void OnDataReceived(const std::shared_ptr<Connection>& conn, IClient::MessageHandler handler) override {
        auto& buf = conn->InputBuffer();
        while (buf.ReadableBytes() >= 4) {
            uint32_t length = 0;
            // Read length header (don't consume yet)
            const char* header = buf.Peek();
            std::copy(header, header + 4, reinterpret_cast<char*>(&length));
            length = ntohl(length);

            // Check sanity
            if (length > 100 * 1024 * 1024) { // 100MB limit
                 // Error or reset? For now, clear buffer to recover
                 buf.RetrieveAll();
                 return;
            }

            if (buf.ReadableBytes() >= 4 + length) {
                // Have full message
                buf.Retrieve(4); // Consume header
                std::string data(buf.Peek(), length);
                buf.Retrieve(length); // Consume body

                StreamEnvelope msg;
                if (msg.ParseFromString(data)) {
                    if (handler) handler(msg);
                }
            } else {
                // Wait for more data
                break;
            }
        }
    }
};

}
}
}

#endif
