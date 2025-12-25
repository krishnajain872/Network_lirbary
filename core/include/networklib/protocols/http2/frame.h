#ifndef NETWORKLIB_PROTOCOLS_HTTP2_FRAME_H
#define NETWORKLIB_PROTOCOLS_HTTP2_FRAME_H

#include <cstdint>
#include <vector>
#include <string>

namespace networklib {
namespace protocols {
namespace http2 {

enum class FrameType : uint8_t {
    DATA = 0x0,
    HEADERS = 0x1,
    PRIORITY = 0x2,
    RST_STREAM = 0x3,
    SETTINGS = 0x4,
    PUSH_PROMISE = 0x5,
    PING = 0x6,
    GOAWAY = 0x7,
    WINDOW_UPDATE = 0x8,
    CONTINUATION = 0x9
};

enum class FrameFlags : uint8_t {
    NONE = 0x0,
    END_STREAM = 0x1,
    END_HEADERS = 0x4,
    PADDED = 0x8,
    PRIORITY = 0x20
};

struct FrameHeader {
    uint32_t length; // 24 bits
    FrameType type;
    uint8_t flags;
    uint32_t stream_id; // 31 bits
};

struct Frame {
    FrameHeader header;
    std::vector<uint8_t> payload;
};

} // namespace http2
} // namespace protocols
} // namespace networklib

#endif // NETWORKLIB_PROTOCOLS_HTTP2_FRAME_H
