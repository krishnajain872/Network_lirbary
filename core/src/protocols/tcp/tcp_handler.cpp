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

void TcpHandler::OnConnection(const core::Connection::Ptr& conn) {
    (void)conn;
}

void TcpHandler::OnMessage(const core::Connection::Ptr& conn) {
    auto& buf = conn->InputBuffer();

    // Check rate limit before processing? Or process and then check?
    // Processing might be expensive, so check first?
    // But OnMessage is called when data is available.
    // RateLimiter is typically for request count.

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
                        stream_handler_(req, resp, nullptr); // nullptr for ctx for now
                    } else {
                        // Echo payload if no handler
                         if (req.has_payload()) {
                             *resp.mutable_payload() = req.payload();
                         }
                    }

                    // Send response
                    std::string payload = resp.SerializeAsString();
                    uint32_t resp_len = htonl(static_cast<uint32_t>(payload.size()));
                    std::string frame;
                    frame.append(reinterpret_cast<const char*>(&resp_len), sizeof(resp_len));
                    frame.append(payload);
                    conn->Send(frame);
                }
             } else {
                 // Rate limited - drop or error?
                 // For now just consume and drop
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
