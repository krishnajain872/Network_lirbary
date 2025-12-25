#include "networklib/protocols/quic/quic_handler.h"
#include <iostream>
#include <sstream>

namespace networklib {
namespace protocols {
namespace quic {

// Mock QUIC Implementation
// Simulates QUIC streams over a transport layer (abstracted here)
// In reality, this would use ngtcp2 or OpenSSL QUIC

struct QuicStream {
    uint64_t stream_id;
    std::string buffer;
};

void QuicHandler::OnConnection(std::shared_ptr<core::Connection> conn) {
    std::cout << "[QUIC] Handshake simulated. Connection established." << std::endl;
    // In real QUIC, we'd negotiate keys here
}

void QuicHandler::OnData(std::shared_ptr<core::Connection> conn, const std::vector<uint8_t>& data) {
    // Basic packet parser simulation
    // Format: [Type:1][StreamID:8][Payload]
    if (data.size() < 9) return;

    uint8_t type = data[0];
    uint64_t stream_id = 0;
    // Basic byte copy for stream ID simulation
    // This is purely to demonstrate handling "streams" different from TCP bytes
    
    std::string payload(data.begin() + 9, data.end());
    
    if (type == 0x01) { // STREAM frame
        std::cout << "[QUIC] Stream " << stream_id << " Data: " << payload << std::endl;
        // Echo back
        std::string response = "QUIC-ACK: " + payload;
        conn->Send(response); 
    }
}

void QuicHandler::OnDisconnect(std::shared_ptr<core::Connection> conn) {
    std::cout << "[QUIC] Session Closed." << std::endl;
}

} // namespace quic
} // namespace protocols
} // namespace networklib
