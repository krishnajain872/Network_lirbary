#include "networklib/protocols/http2/frame_parser.h"
#include <cstring>
#include <arpa/inet.h>

namespace networklib {
namespace protocols {
namespace http2 {

FrameParser::FrameParser() : state_(State::kReadingHeader) {}

bool FrameParser::Parse(core::memory::Buffer& buf, Frame& out_frame) {
    if (state_ == State::kReadingHeader) {
        if (buf.ReadableBytes() < 9) {
            return false; // Need 9 bytes for header
        }

        const uint8_t* data = reinterpret_cast<const uint8_t*>(buf.Peek());
        
        // Length (24 bits)
        uint32_t length = (data[0] << 16) | (data[1] << 8) | data[2];
        
        // Type (8 bits)
        FrameType type = static_cast<FrameType>(data[3]);
        
        // Flags (8 bits)
        uint8_t flags = data[4];
        
        // Stream ID (31 bits)
        uint32_t stream_id = (data[5] << 24) | (data[6] << 16) | (data[7] << 8) | data[8];
        stream_id &= 0x7FFFFFFF; // Clear reserved bit

        current_header_ = {length, type, flags, stream_id};
        buf.Retrieve(9);
        state_ = State::kReadingPayload;
    }

    if (state_ == State::kReadingPayload) {
        if (buf.ReadableBytes() < current_header_.length) {
            return false;
        }

        out_frame.header = current_header_;
        out_frame.payload.resize(current_header_.length);
        if (current_header_.length > 0) {
            std::memcpy(out_frame.payload.data(), buf.Peek(), current_header_.length);
            buf.Retrieve(current_header_.length);
        }

        state_ = State::kReadingHeader; // Reset for next frame
        return true;
    }

    return false;
}

} // namespace http2
} // namespace protocols
} // namespace networklib
