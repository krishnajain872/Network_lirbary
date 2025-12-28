#include "network/protocols/http2/stream.h"

namespace networklib {
namespace protocols {
namespace http2 {

Stream::Stream(uint32_t id) : id_(id), state_(State::kIdle) {}

void Stream::AppendData(const uint8_t* data, size_t len) {
    data_buffer_.Append(reinterpret_cast<const char*>(data), len);
}

} // namespace http2
} // namespace protocols
} // namespace networklib
