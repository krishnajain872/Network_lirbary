#include "network/protocols/http2/hpack.h"

namespace networklib {
namespace protocols {
namespace http2 {

// Simplified Static Table Mapping for common fields
// RFC 7541 Static Table
// 2: :method: GET
// 3: :method: POST
// 4: :path: /
// 6: :scheme: http
// 7: :scheme: https
// 8: :status: 200
// 14: :status: 500
// 58: user-agent
// ...

void HpackEncoder::EncodeInteger(std::vector<uint8_t>& out, uint32_t value, uint8_t n) {
    uint8_t mask = (1 << n) - 1;
    if (value < mask) {
        out.back() |= value;
    } else {
        out.back() |= mask;
        value -= mask;
        while (value >= 128) {
            out.push_back((value & 0x7F) | 0x80);
            value >>= 7;
        }
        out.push_back(value);
    }
}

void HpackEncoder::EncodeString(std::vector<uint8_t>& out, const std::string& str) {
    // Length prefix (H=0, 7 bits)
    out.push_back(0); // Placeholder or start
    EncodeInteger(out, str.size(), 7);
    // String bytes
    for (char c : str) out.push_back(static_cast<uint8_t>(c));
}

std::vector<uint8_t> HpackEncoder::Encode(const std::map<std::string, std::string>& headers) {
    std::vector<uint8_t> buffer;

    for (const auto& [name, value] : headers) {
        if (name == ":method" && value == "POST") {
            buffer.push_back(0x80 | 3); // Indexed, Index 3
        } else if (name == ":scheme" && value == "http") {
            buffer.push_back(0x80 | 6);
        } else if (name == ":status" && value == "200") {
            buffer.push_back(0x80 | 8);
        } else {
            // Literal Header Field without Indexing (0000xxxx)
            // Or Literal Header Field with Incremental Indexing (01xxxxxx) -> 0x40
            // Let's use Without Indexing (0x00) for simplicity to avoid managing dynamic table state
            // Format: 0000 Index(4)
            // If Name is in static table (e.g. :path = 4), we can ref it.

            uint8_t prefix = 0x00; // Literal without indexing

            if (name == ":path") {
                buffer.push_back(prefix | 4); // Index 4 (:path)
                EncodeString(buffer, value);
            } else if (name == ":authority") {
                buffer.push_back(prefix | 1); // Index 1 (:authority)
                EncodeString(buffer, value);
            } else if (name == "content-type") {
                // content-type is empty in static table? No.
                // But we can just send as literal name/literal value
                buffer.push_back(prefix | 0); // New Name
                EncodeString(buffer, name);
                EncodeString(buffer, value);
            } else {
                buffer.push_back(prefix | 0); // New Name
                EncodeString(buffer, name);
                EncodeString(buffer, value);
            }
        }
    }
    return buffer;
}

} // namespace http2
} // namespace protocols
} // namespace networklib
