#ifndef NETWORKLIB_PROTOCOLS_HTTP2_STREAM_H
#define NETWORKLIB_PROTOCOLS_HTTP2_STREAM_H

#include <cstdint>
#include <vector>
#include <string>
#include "network/core/memory/buffer.h"

namespace networklib {
namespace protocols {
namespace http2 {

class Stream {
public:
    enum class State {
        kIdle,
        kOpen,
        kHalfClosedRemote,
        kHalfClosedLocal,
        kClosed
    };

    Stream(uint32_t id);

    uint32_t Id() const { return id_; }
    State GetState() const { return state_; }
    void SetState(State state) { state_ = state; }

    void AppendData(const uint8_t* data, size_t len);
    core::memory::Buffer& DataBuffer() { return data_buffer_; }

private:
    uint32_t id_;
    State state_;
    core::memory::Buffer data_buffer_;
};

} // namespace http2
} // namespace protocols
} // namespace networklib

#endif // NETWORKLIB_PROTOCOLS_HTTP2_STREAM_H
