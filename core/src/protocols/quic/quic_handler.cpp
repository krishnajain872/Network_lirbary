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

void QuicHandler::OnConnection(const networklib::core::Connection::Ptr& conn) {
    std::cout << "[QUIC] Handshake simulated. Connection established." << std::endl;
    // In real QUIC, we'd negotiate keys here
}

void QuicHandler::OnMessage(const networklib::core::Connection::Ptr& conn) {
    // Basic packet parser simulation from InputBuffer
    auto& buffer = conn->InputBuffer();

    // Format: [Type:1][StreamID:8][Payload]
    if (buffer.ReadableBytes() < 9) return;

    // Peek/Read logic here. For simplicity, we just read everything
    std::string data(buffer.Peek(), buffer.ReadableBytes());
    buffer.RetrieveAll();

    uint8_t type = data[0];
    // uint64_t stream_id = 0; // Simulated
    
    std::string payload = data.substr(9);
    
    if (type == 0x01) { // STREAM frame
        std::cout << "[QUIC] Stream 0 Data: " << payload << std::endl;
        // Echo back
        std::string response = "QUIC-ACK: " + payload;
        conn->Send(response); 
    }
}

} // namespace quic
} // namespace protocols
} // namespace networklib
