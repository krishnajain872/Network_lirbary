#include "network/protocols/rpc/rpc_handler.h"
#include "stream_envelope.pb.h"
#include "logger/logging.h"
#include <iostream>
#include <arpa/inet.h>
#include <cstring>

namespace networklib {
namespace protocols {
namespace rpc {

class RpcStreamContext : public networklib::IStreamContext {
public:
    RpcStreamContext(core::Connection::Ptr conn) : conn_(conn) {}

    void Write(const StreamEnvelope& msg) override {
        if (auto conn = conn_.lock()) {
            std::string payload = msg.SerializeAsString();
            uint32_t resp_len = htonl(static_cast<uint32_t>(payload.size()));
            std::string frame;
            frame.append(reinterpret_cast<const char*>(&resp_len), 4);
            frame.append(payload);
            conn->Send(frame);
        }
    }

    void Write(const std::string& data) override {
        // Raw write (assuming already framed or raw bytes allowed?)
        // For RPC, we enforce framing. So we frame the raw data.
        if (auto conn = conn_.lock()) {
            uint32_t resp_len = htonl(static_cast<uint32_t>(data.size()));
            std::string frame;
            frame.append(reinterpret_cast<const char*>(&resp_len), 4);
            frame.append(data);
            conn->Send(frame);
        }
    }

    void Close() override {
        if (auto conn = conn_.lock()) {
            conn->ForceClose();
        }
    }

private:
    std::weak_ptr<core::Connection> conn_;
};

void RpcHandler::OnConnection(const core::Connection::Ptr& conn) {
    // Initial state setup if needed
    (void)conn;
}

void RpcHandler::OnMessage(const core::Connection::Ptr& conn) {
    auto& buf = conn->InputBuffer();
    
    // Loop to handle multiple messages
    while (true) {
        if (buf.ReadableBytes() < 4) {
            break; // Wait for length prefix
        }
        
        // Peek length (Big Endian)
        const char* header = buf.Peek();
        uint32_t length = 0;
        std::memcpy(&length, header, 4);
        length = ntohl(length);
        
        if (buf.ReadableBytes() < 4 + length) {
            break; // Wait for full payload
        }
        
        if (stream_handler_) {
            // Parse Payload
            StreamEnvelope req;
            // Note: ParseFromArray is efficient
            if (req.ParseFromArray(header + 4, length)) {
                StreamEnvelope resp;
                auto ctx = std::make_shared<RpcStreamContext>(conn);
                stream_handler_(req, resp, ctx);

                // If handler populated resp immediately
                if (resp.has_header() || resp.has_payload()) {
                    ctx->Write(resp);
                }
            }
        } else {
            // Legacy Echo Logic (Fallback)
            StreamEnvelope envelope;
            if (envelope.ParseFromArray(header + 4, length)) {
                StreamEnvelope response;
                response.mutable_header()->set_message_type(envelope.header().message_type() + ".response");
                response.mutable_header()->set_correlation_id(envelope.header().message_id());
                response.mutable_payload()->set_data(envelope.payload().data());

                std::string serialized;
                response.SerializeToString(&serialized);

                uint32_t resp_len = htonl(serialized.size());
                char resp_header[4];
                std::memcpy(resp_header, &resp_len, 4);

                conn->Send(resp_header, 4);
                conn->Send(serialized);
            }
        }
        
        buf.Retrieve(4 + length);
    }
}

} // namespace rpc
} // namespace protocols
} // namespace networklib
