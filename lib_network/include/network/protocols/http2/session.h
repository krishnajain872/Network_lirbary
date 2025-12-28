#ifndef NETWORKLIB_PROTOCOLS_HTTP2_SESSION_H
#define NETWORKLIB_PROTOCOLS_HTTP2_SESSION_H

#include <map>
#include <memory>
#include "network/protocols/http2/stream.h"
#include "network/protocols/http2/frame_parser.h"
#include "network/core/connection.h"

namespace networklib {
namespace protocols {
namespace http2 {

class Session {
public:
    Session(core::Connection::Ptr conn);

    void OnData();
    std::shared_ptr<Stream> GetStream(uint32_t id);
    
    // Callback for new streams/messages
    
    // Callback for new streams/messages
    void SetOnStreamData(std::function<void(std::shared_ptr<Stream>, const std::vector<uint8_t>&)> cb) {
        on_stream_data_ = cb;
    }

    /**
     * @brief Sends data on a specific stream (wraps in DATA frame).
     */
    void SendStreamData(uint32_t stream_id, const std::string& data, bool end_stream = false);

private:
    void HandleFrame(const Frame& frame);

    core::Connection::Ptr connection_;
    FrameParser parser_;
    std::map<uint32_t, std::shared_ptr<Stream>> streams_;
    bool preface_received_{false};
    std::function<void(std::shared_ptr<Stream>, const std::vector<uint8_t>&)> on_stream_data_;
};

} // namespace http2
} // namespace protocols
} // namespace networklib

#endif // NETWORKLIB_PROTOCOLS_HTTP2_SESSION_H
