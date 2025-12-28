#ifndef NETWORKLIB_PROTOCOLS_WEBSOCKET_FRAME_H
#define NETWORKLIB_PROTOCOLS_WEBSOCKET_FRAME_H

#include <cstdint>
#include <vector>
#include <string>

namespace networklib {
namespace protocols {
namespace websocket {

enum class OpCode : uint8_t {
    kContinuation = 0x0,
    kText = 0x1,
    kBinary = 0x2,
    kClose = 0x8,
    kPing = 0x9,
    kPong = 0xA
};

struct Frame {
    bool fin;
    OpCode opcode;
    bool masked;
    std::vector<uint8_t> payload;
};

class FrameParser {
public:
    enum class State { kHeader, kPayloadLength, kMaskKey, kPayload };

    FrameParser();
    
    // Returns true if a full frame was parsed
    bool Parse(const char* data, size_t len, size_t& consumed, Frame& out_frame);

private:
    State state_;
    Frame current_frame_;
    uint64_t payload_length_;
    uint8_t mask_key_[4];
    size_t bytes_needed_;
};

} // namespace websocket
} // namespace protocols
} // namespace networklib

#endif // NETWORKLIB_PROTOCOLS_WEBSOCKET_FRAME_H
