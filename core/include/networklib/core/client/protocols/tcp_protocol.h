#ifndef NETWORKLIB_CORE_CLIENT_PROTOCOLS_TCP_PROTOCOL_H
#define NETWORKLIB_CORE_CLIENT_PROTOCOLS_TCP_PROTOCOL_H

#include "networklib/core/client/client_protocol.h"
#include "stream_envelope.pb.h"

namespace networklib {
namespace core {
namespace client {

class TcpClientProtocol : public IClientProtocol {
public:
    void OnConnect(const std::shared_ptr<Connection>& /*conn*/) override {}

    void Send(const std::shared_ptr<Connection>& conn, const StreamEnvelope& envelope) override {
        conn->Send(envelope.SerializeAsString());
    }

    void OnDataReceived(const std::shared_ptr<Connection>& conn, IClient::MessageHandler handler) override {
        auto& buf = conn->InputBuffer();
        if (buf.ReadableBytes() > 0) {
            // Naive TCP: Assume one packet one message (Broken for stream, but consistent with current impl)
            // Real impl needs length prefix.
            // Try parse
            StreamEnvelope msg;
            std::string data(buf.Peek(), buf.ReadableBytes());
            if (msg.ParseFromString(data)) {
                if (handler) handler(msg);
                buf.RetrieveAll();
            } else {
                // Wait? Or consume all as raw?
                // Fallback
                buf.RetrieveAll();
            }
        }
    }
};

}
}
}

#endif
