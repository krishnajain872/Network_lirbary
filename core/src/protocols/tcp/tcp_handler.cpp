#include "networklib/protocols/tcp/tcp_handler.h"
#include "networklib/constants/limits.h"
#include "stream_envelope.pb.h"
#include <unistd.h>
#include <iostream>
#include <errno.h>
#include <arpa/inet.h>

namespace networklib {
namespace protocols {
namespace tcp {

// Simple implementation of IStreamContext for TCP
class TcpStreamContext : public IStreamContext {
public:
    TcpStreamContext(core::Connection::Ptr conn) : conn_(conn) {}

    void Write(const StreamEnvelope& msg) override {
        if (auto conn = conn_.lock()) {
            std::string payload = msg.SerializeAsString();
            if (payload.empty()) return; // Don't send empty frames

            uint32_t resp_len = htonl(static_cast<uint32_t>(payload.size()));
            std::string frame;
            frame.append(reinterpret_cast<const char*>(&resp_len), sizeof(resp_len));
            frame.append(payload);
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

void TcpHandler::OnConnection(const core::Connection::Ptr& conn) {
    (void)conn;
}

void TcpHandler::OnMessage(const core::Connection::Ptr& conn) {
    auto& buf = conn->InputBuffer();

    while (buf.ReadableBytes() >= 4) {
        uint32_t length = 0;
        const char* header = buf.Peek();
        std::copy(header, header + 4, reinterpret_cast<char*>(&length));
        length = ntohl(length);

        if (length > 100 * 1024 * 1024) { // Sanity check
             LOG_ERROR("Invalid message length %u. Closing connection.", length);
             conn->ForceClose(); // Force close to prevent desync
             return;
        }

        if (buf.ReadableBytes() >= 4 + length) {
             if (CheckRateLimit()) {
                buf.Retrieve(4);
                std::string data(buf.Peek(), length);
                buf.Retrieve(length);

                StreamEnvelope req;
                if (req.ParseFromString(data)) {
                    StreamEnvelope resp;
                    // Prepare response skeleton
                    if (req.has_header()) {
                        resp.mutable_header()->set_correlation_id(req.header().message_id());
                        resp.mutable_header()->set_message_id(req.header().message_id() + "_resp"); // Simple ID gen
                    }

                    if (stream_handler_) {
                        // Create a context for this connection
                        auto ctx = std::make_shared<TcpStreamContext>(conn);
                        stream_handler_(req, resp, ctx);
                    } else {
                        // Echo payload if no handler
                         if (req.has_payload()) {
                             *resp.mutable_payload() = req.payload();
                         }
                    }

                    // Send response ONLY if it has content (header or payload)
                    if (resp.has_header() || resp.has_payload() || resp.ByteSizeLong() > 0) {
                        std::string payload = resp.SerializeAsString();
                        if (!payload.empty()) {
                            uint32_t resp_len = htonl(static_cast<uint32_t>(payload.size()));
                            std::string frame;
                            frame.append(reinterpret_cast<const char*>(&resp_len), sizeof(resp_len));
                            frame.append(payload);
                            conn->Send(frame);
                        }
                    }
                }
             } else {
                 // Rate Limited
                 // Use Fd instead of PeerAddress since PeerAddress doesn't exist
                 LOG_WARN("Rate limit exceeded for fd %d. Dropping message.", conn->Fd());
                 buf.Retrieve(4 + length);
             }
        } else {
            break;
        }
    }
}

} // namespace tcp
} // namespace protocols
} // namespace networklib
