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
             buf.RetrieveAll();
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
                        // Note: Creating a new shared_ptr every time is inefficient but safe for now.
                        // Ideally we should cache it in the Connection context or similar.
                        auto ctx = std::make_shared<TcpStreamContext>(conn);
                        stream_handler_(req, resp, ctx);
                    } else {
                        // Echo payload if no handler
                         if (req.has_payload()) {
                             *resp.mutable_payload() = req.payload();
                         }
                    }

                    // Send response ONLY if it was populated (and not just for async handling)
                    // If stream_handler handled it entirely async (e.g. broadcast), resp might be empty/unused?
                    // But typically request/response pattern expects a response.
                    // If the handler didn't touch resp, we might still want to send something?
                    // For now, we preserve existing behavior: send whatever is in resp.

                    std::string payload = resp.SerializeAsString();
                    uint32_t resp_len = htonl(static_cast<uint32_t>(payload.size()));
                    std::string frame;
                    frame.append(reinterpret_cast<const char*>(&resp_len), sizeof(resp_len));
                    frame.append(payload);
                    conn->Send(frame);
                }
             } else {
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
