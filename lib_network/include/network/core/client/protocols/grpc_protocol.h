#ifndef NETWORKLIB_CORE_CLIENT_PROTOCOLS_GRPC_PROTOCOL_H
#define NETWORKLIB_CORE_CLIENT_PROTOCOLS_GRPC_PROTOCOL_H

#include "network/core/client/client_protocol.h"
#include "stream_envelope.pb.h"
#include "network/protocols/grpc/grpc_codec.h"
#include "logger/logging.h"
#include <vector>
#include <cstring>

namespace networklib {
namespace core {
namespace client {

// Minimal HTTP/2 Client Implementation for gRPC
class GrpcClientProtocol : public IClientProtocol {
public:
    void OnConnect(const std::shared_ptr<Connection>& conn) override {
        // 1. Send Connection Preface
        std::string preface = "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n";
        conn->Send(preface);

        // 2. Send Initial SETTINGS Frame (Empty)
        // Length: 0, Type: 4 (SETTINGS), Flags: 0, Stream: 0
        uint8_t settings[9] = {0, 0, 0, 4, 0, 0, 0, 0, 0};
        conn->Send(reinterpret_cast<char*>(settings), 9);

        logging::Logger::Log(logging::LogLevel::Info, __FILE__, __LINE__, __FUNCTION__, "Sent HTTP/2 Preface and SETTINGS");
    }

    void Send(const std::shared_ptr<Connection>& conn, const StreamEnvelope& envelope) override {
        // gRPC over HTTP/2 requires:
        // 1. HEADERS frame (Stream ID 1, End Headers)
        // 2. DATA frame (Stream ID 1, End Stream) - containing gRPC Length-Prefixed Message

        // Construct basic HEADERS frame block (Mock HPACK or Empty)
        // Since our Server GrpcHandler MOCKS the path if missing, sending empty headers might work
        // if the FrameParser accepts it.
        // Length 0 HEADERS frame means empty header list.

        // Frame Header: Length(3), Type(1), Flags(1), Stream(4)
        // Type 1 = HEADERS
        // Flags 0x4 = END_HEADERS
        // Stream 1
        uint8_t headers[9] = {0, 0, 0, 1, 0x4, 0, 0, 0, 1};
        conn->Send(reinterpret_cast<char*>(headers), 9);

        // DATA Frame
        std::vector<char> grpc_bytes = protocols::grpc::GrpcCodec::Encode(envelope.SerializeAsString());
        uint32_t len = grpc_bytes.size();

        // Type 0 = DATA
        // Flags 0x1 = END_STREAM (Unary call assumption)
        uint8_t data_header[9];
        data_header[0] = (len >> 16) & 0xFF;
        data_header[1] = (len >> 8) & 0xFF;
        data_header[2] = len & 0xFF;
        data_header[3] = 0; // DATA
        data_header[4] = 0x1; // END_STREAM
        data_header[5] = 0;
        data_header[6] = 0;
        data_header[7] = 0;
        data_header[8] = 1; // Stream 1

        conn->Send(reinterpret_cast<char*>(data_header), 9);
        conn->Send(std::string(grpc_bytes.begin(), grpc_bytes.end()));
    }

    void OnDataReceived(const std::shared_ptr<Connection>& conn, IClient::MessageHandler handler) override {
        auto& buf = conn->InputBuffer();

        while (buf.ReadableBytes() >= 9) {
            const uint8_t* p = reinterpret_cast<const uint8_t*>(buf.Peek());
            uint32_t len = (p[0] << 16) | (p[1] << 8) | p[2];
            uint8_t type = p[3];
            uint8_t flags = p[4];
            uint32_t stream_id = (p[5] & 0x7F) << 24 | (p[6] << 16) | (p[7] << 8) | p[8];

            if (buf.ReadableBytes() >= 9 + len) {
                // Handle Control Frames
                if (type == 4) { // SETTINGS
                    if ((flags & 0x1) == 0) {
                        // Received SETTINGS, must ACK
                        logging::Logger::Log(logging::LogLevel::Info, __FILE__, __LINE__, __FUNCTION__, "Received SETTINGS, sending ACK");
                        uint8_t ack[9] = {0, 0, 0, 4, 0x1, 0, 0, 0, 0};
                        conn->Send(reinterpret_cast<char*>(ack), 9);
                    }
                }
                else if (type == 0) { // DATA
                    core::memory::Buffer payload_buf;
                    payload_buf.Append(buf.Peek() + 9, len);

                    std::string payload;
                    bool compressed;
                    if (protocols::grpc::GrpcCodec::Decode(payload_buf, payload, compressed)) {
                        StreamEnvelope env;
                        // For generic client, payload IS StreamEnvelope serialized
                        if (env.ParseFromString(payload)) {
                            if (handler) handler(env);
                        }
                    }
                }

                // Consume frame
                buf.Retrieve(9 + len);
            } else {
                break; // Wait for full frame
            }
        }
    }
};

}
}
}

#endif
