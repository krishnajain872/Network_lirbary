#ifndef NETWORKLIB_PROTOCOLS_HTTP2_FRAME_PARSER_H
#define NETWORKLIB_PROTOCOLS_HTTP2_FRAME_PARSER_H

#include "network/protocols/http2/frame.h"
#include "network/core/memory/buffer.h"
#include "network/utils/result.h"

namespace networklib {
namespace protocols {
namespace http2 {

class FrameParser {
public:
    enum class State {
        kReadingHeader,
        kReadingPayload
    };

    FrameParser();

    /**
     * @brief Attempts to parse a frame from the buffer.
     * 
     * @param buf Input buffer.
     * @param out_frame Output frame if parsing is successful.
     * @return true if a frame was parsed, false if more data is needed.
     */
    bool Parse(core::memory::Buffer& buf, Frame& out_frame);

private:
    State state_;
    FrameHeader current_header_;
};

} // namespace http2
} // namespace protocols
} // namespace networklib

#endif // NETWORKLIB_PROTOCOLS_HTTP2_FRAME_PARSER_H
