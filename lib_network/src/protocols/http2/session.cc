#include "network/protocols/http2/session.h"
#include <iostream>
#include <cstring>

namespace networklib {
namespace protocols {
namespace http2 {

static const char kClientPreface[] = "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n";
static const size_t kPrefaceLen = 24;

Session::Session(core::Connection::Ptr conn) : connection_(conn) {}

void Session::OnData() {
    auto& buf = connection_->InputBuffer();
    
    if (!preface_received_) {
        if (buf.ReadableBytes() >= kPrefaceLen) {
            // Check preface
            if (std::memcmp(buf.Peek(), kClientPreface, kPrefaceLen) == 0) {
                buf.Retrieve(kPrefaceLen);
                preface_received_ = true;
            } else {
                // Invalid preface, close connection
                connection_->ForceClose();
                return;
            }
        } else {
            return; // Wait for more data
        }
    }

    Frame frame;
    while (parser_.Parse(buf, frame)) {
        HandleFrame(frame);
    }
}

std::shared_ptr<Stream> Session::GetStream(uint32_t id) {
    auto it = streams_.find(id);
    if (it != streams_.end()) {
        return it->second;
    }
    // Create new stream (simplified for now)
    auto stream = std::make_shared<Stream>(id);
    streams_[id] = stream;
    return stream;
}

void Session::HandleFrame(const Frame& frame) {
    // Basic dispatcher
    if (frame.header.stream_id > 0) {
        auto stream = GetStream(frame.header.stream_id);
        if (frame.header.type == FrameType::DATA) {
            stream->AppendData(frame.payload.data(), frame.payload.size());
            // Notify higher layer
            if (on_stream_data_) {
                on_stream_data_(stream, frame.payload);
            }
        }
    }
    // Handle control frames (SETTINGS, PING) here
}

void Session::SendStreamData(uint32_t stream_id, const std::string& data, bool end_stream) {
    // Basic DATA Frame construction
    // Length (3 bytes), Type (1 byte), Flags (1 byte), Stream ID (4 bytes)
    // 0x00 = DATA type
    // 0x01 = END_STREAM flag
    
    uint32_t length = data.size();
    uint8_t flags = end_stream ? 0x01 : 0x00;
    
    std::vector<char> header(9);
    header[0] = (length >> 16) & 0xFF;
    header[1] = (length >> 8) & 0xFF;
    header[2] = length & 0xFF;
    header[3] = 0x00; // DATA
    header[4] = flags;
    header[5] = (stream_id >> 24) & 0x7F; // Reserved bit 0
    header[6] = (stream_id >> 16) & 0xFF;
    header[7] = (stream_id >> 8) & 0xFF;
    header[8] = stream_id & 0xFF;
    
    connection_->Send(header.data(), 9);
    if (!data.empty()) {
        connection_->Send(data);
    }
}

} // namespace http2
} // namespace protocols
} // namespace networklib
