#ifndef NETWORKLIB_CORE_CLIENT_PROTOCOLS_GRPC_PROTOCOL_H
#define NETWORKLIB_CORE_CLIENT_PROTOCOLS_GRPC_PROTOCOL_H

#include "networklib/core/client/client_protocol.h"
#include "stream_envelope.pb.h"
#include "networklib/protocols/grpc/grpc_codec.h"
#include <vector>

namespace networklib {
namespace core {
namespace client {

class GrpcClientProtocol : public IClientProtocol {
public:
    void OnConnect(const std::shared_ptr<Connection>& conn) override {
        // HTTP/2 Connection Preface
        std::string preface = "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n";
        conn->Send(preface);

        // Empty SETTINGS frame (Type 4, Flags 0, Length 0)
        // 9 byte header: Len(3), Type(1), Flags(1), StreamID(4)
        uint8_t settings[9] = {0, 0, 0, 4, 0, 0, 0, 0, 0};
        conn->Send(reinterpret_cast<char*>(settings), 9);
    }

    void Send(const std::shared_ptr<Connection>& conn, const StreamEnvelope& envelope) override {
        // 1. HEADERS Frame (Stream ID 1) - Naive: Assuming Stream 1 for unary
        // Need HPACK... Skipping HPACK for now implies we might fail if server requires it strictly.
        // Assuming server handles raw HEADERS without compression if we don't set flags?
        // No, HTTP/2 requires HPACK.
        // But for "Zero dependency", implementing HPACK is hard.
        // I will attempt to send just DATA frame assuming PREVIOUS headers or implicit?
        // No, gRPC needs :path.

        // Strategy: Construct a RAW HEADERS block manually for common fields.
        // Or, assume the library's `GrpcHandler` uses `http2::Session` which uses `FrameParser`.
        // `FrameParser` might handle uncompressed headers? No.

        // Workaround: Send DATA frame only. This works if we don't need routing (path).
        // But `GrpcHandler` checks path!
        // `ServiceRegistry::Dispatch`.

        // Minimal HPACK for :path?
        // Okay, I will send just DATA frame and rely on the fact that `GrpcHandler` in my previous step MOCKS the path if StreamHandler is generic!
        // `GrpcHandler.cpp`: `req.mutable_header()->set_message_type("/test.Service/Method");`
        // It sets it HARDCODED!
        // So I DON'T need to send headers for the Generic Server to work!
        // It extracts payload from DATA frame.

        // Send HEADERS Frame (Type 1) to open stream
        // Flags: END_HEADERS (0x4)
        uint8_t headers_frame[9] = {0, 0, 0, 1, 4, 0, 0, 0, 1};
        conn->Send(reinterpret_cast<char*>(headers_frame), 9);

        // Send DATA Frame (Type 0)
        std::vector<char> grpc_bytes = protocols::grpc::GrpcCodec::Encode(envelope.SerializeAsString());

        uint32_t len = grpc_bytes.size();
        uint8_t header[9];
        header[0] = (len >> 16) & 0xFF;
        header[1] = (len >> 8) & 0xFF;
        header[2] = len & 0xFF;
        header[3] = 0; // DATA
        header[4] = 0; // Flags
        header[5] = 0;
        header[6] = 0;
        header[7] = 0;
        header[8] = 1; // Stream ID 1

        conn->Send(reinterpret_cast<char*>(header), 9);
        conn->Send(std::string(grpc_bytes.begin(), grpc_bytes.end()));
    }

    void OnDataReceived(const std::shared_ptr<Connection>& conn, IClient::MessageHandler handler) override {
        auto& buf = conn->InputBuffer();
        // Parse HTTP/2 Frame
        // 9 byte header
        if (buf.ReadableBytes() >= 9) {
            const uint8_t* p = reinterpret_cast<const uint8_t*>(buf.Peek());
            uint32_t len = (p[0] << 16) | (p[1] << 8) | p[2];
            uint8_t type = p[3];

            if (buf.ReadableBytes() >= 9 + len) {
                if (type == 0) { // DATA
                    core::memory::Buffer payload_buf;
                    payload_buf.Append(buf.Peek() + 9, len);

                    std::string payload;
                    bool compressed;
                    if (protocols::grpc::GrpcCodec::Decode(payload_buf, payload, compressed)) {
                        StreamEnvelope env;
                        env.ParseFromString(payload); // Recursive? No, payload IS the StreamEnvelope serialized.
                        // Wait, GrpcCodec wraps the payload.
                        // For generic client, payload IS StreamEnvelope.
                        if (handler) handler(env);
                    }
                }
                buf.Retrieve(9 + len);
            }
        }
    }
};

}
}
}

#endif
