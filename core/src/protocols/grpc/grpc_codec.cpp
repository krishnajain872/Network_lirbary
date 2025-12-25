#include "networklib/protocols/grpc/grpc_codec.h"
#include <arpa/inet.h>
#include <cstring>

namespace networklib {
namespace protocols {
namespace grpc {

std::vector<char> GrpcCodec::Encode(const std::string& payload, bool compressed) {
    std::vector<char> result(kHeaderSize + payload.size());
    
    // Compression flag
    result[0] = compressed ? 1 : 0;
    
    // Length (Big Endian)
    uint32_t length = htonl(static_cast<uint32_t>(payload.size()));
    std::memcpy(&result[1], &length, 4);
    
    // Payload
    if (!payload.empty()) {
        std::memcpy(&result[kHeaderSize], payload.data(), payload.size());
    }
    
    return result;
}

bool GrpcCodec::Decode(core::memory::Buffer& buf, std::string& out_payload, bool& out_compressed) {
    if (buf.ReadableBytes() < kHeaderSize) {
        return false;
    }

    const char* data = buf.Peek();
    out_compressed = (data[0] != 0);
    
    uint32_t length = 0;
    std::memcpy(&length, &data[1], 4);
    length = ntohl(length); // Network to Host

    if (buf.ReadableBytes() < kHeaderSize + length) {
        return false; // Wait for full payload
    }

    out_payload.assign(data + kHeaderSize, length);
    buf.Retrieve(kHeaderSize + length);
    return true;
}

} // namespace grpc
} // namespace protocols
} // namespace networklib
