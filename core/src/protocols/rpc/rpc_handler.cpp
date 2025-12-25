#include "networklib/protocols/rpc/rpc_handler.h"
#include "stream_envelope.pb.h"
#include "networklib/logger.h"
#include <iostream>
#include <arpa/inet.h>
#include <cstring>

namespace networklib {
namespace protocols {
namespace rpc {

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
        
        // Parse Payload
        StreamEnvelope envelope;
        if (envelope.ParseFromArray(header + 4, length)) {
            // Dispatch
            logging::Logger::Log(logging::LogLevel::Info, __FILE__, __LINE__, __FUNCTION__, "RPC Recv: %s", envelope.header().message_type().c_str());
            
            // Simple Echo Logic for Phase 17
            StreamEnvelope response;
            response.mutable_header()->set_message_type(envelope.header().message_type() + ".response");
            response.mutable_header()->set_correlation_id(envelope.header().message_id());
            response.mutable_payload()->set_data(envelope.payload().data());
            
            std::string serialized;
            response.SerializeToString(&serialized);
            
            // Send back (Length + Data)
            uint32_t resp_len = htonl(serialized.size());
            char resp_header[4];
            std::memcpy(resp_header, &resp_len, 4);
            
            conn->Send(resp_header, 4);
            conn->Send(serialized);
        }
        
        buf.Retrieve(4 + length);
    }
}

} // namespace rpc
} // namespace protocols
} // namespace networklib
