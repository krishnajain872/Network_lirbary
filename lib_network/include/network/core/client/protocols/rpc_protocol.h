#ifndef NETWORKLIB_CORE_CLIENT_PROTOCOLS_RPC_PROTOCOL_H
#define NETWORKLIB_CORE_CLIENT_PROTOCOLS_RPC_PROTOCOL_H

#include "network/core/client/client_protocol.h"
#include "stream_envelope.pb.h"
#include "logger/logging.h"
#include <arpa/inet.h>
#include <atomic>
#include <map>
#include <mutex>

namespace networklib {
namespace core {
namespace client {

class RpcClientProtocol : public IClientProtocol {
public:
    void OnConnect(const std::shared_ptr<Connection>& /*conn*/) override {
        // RPC might need a handshake in future, but simple length-prefix needs none.
    }

    void Send(const std::shared_ptr<Connection>& conn, const StreamEnvelope& envelope) override {
        StreamEnvelope msg = envelope; // Copy to modify
        if (!msg.has_header()) {
            msg.mutable_header()->set_message_type("rpc.generic");
        }

        // Ensure Correlation ID for "Actual RPC" behavior
        if (msg.header().correlation_id().empty()) {
            msg.mutable_header()->set_correlation_id(GenerateId());
        }

        std::string payload = msg.SerializeAsString();
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
            const char* header = buf.Peek();
            std::copy(header, header + 4, reinterpret_cast<char*>(&length));
            length = ntohl(length);

            if (length > 100 * 1024 * 1024) {
                 logging::Logger::Log(logging::LogLevel::Error, __FILE__, __LINE__, __FUNCTION__, "RPC Error: Invalid length %u", length);
                 conn->ForceClose();
                 return;
            }

            if (buf.ReadableBytes() >= 4 + length) {
                buf.Retrieve(4);
                std::string data(buf.Peek(), length);
                buf.Retrieve(length);

                StreamEnvelope msg;
                if (msg.ParseFromString(data)) {
                    // In a full RPC client, we might match correlation_id to pending requests here.
                    // For now, we pass it to the handler (ScenarioEngine) which does the Expect.
                    if (handler) handler(msg);
                }
            } else {
                break;
            }
        }
    }

private:
    std::string GenerateId() {
        static std::atomic<uint64_t> id_counter{1};
        return "req-" + std::to_string(id_counter.fetch_add(1));
    }
};

}
}
}

#endif
