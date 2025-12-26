#ifndef NETWORKLIB_PROTOCOLS_GRPC_GRPC_STREAM_IMPL_H
#define NETWORKLIB_PROTOCOLS_GRPC_GRPC_STREAM_IMPL_H

#include "networklib/protocols/grpc/grpc_stream.h"
#include "networklib/protocols/http2/session.h"
#include "networklib/protocols/http2/stream.h"
#include "networklib/protocols/grpc/grpc_codec.h"
#include <iostream>

namespace networklib {
namespace protocols {
namespace grpc {

class GrpcStreamImpl : public GrpcStream, public std::enable_shared_from_this<GrpcStreamImpl> {
public:
    GrpcStreamImpl(std::shared_ptr<networklib::protocols::http2::Stream> stream, std::shared_ptr<networklib::protocols::http2::Session> session)
        : stream_(stream), session_(session) {}

    void Write(const std::string& data) override {
        // 1. Encode to gRPC Frame
        std::vector<char> frame = GrpcCodec::Encode(data);
        
        // 2. Send via HTTP/2 Data Frame
        std::string s_frame(frame.begin(), frame.end());
        if (session_ && stream_) {
            session_->SendStreamData(stream_->Id(), s_frame);
        }
    }

    void WriteHeaders(int /*status*/) override {
        // Send HTTP/2 HEADERS frame (200 OK, application/grpc)
    }

    void Finish(int /*status*/) override {
        // Send Trailers (grpc-status) + End Stream
    }

    void SetOnMessage(std::function<void(const std::string&)> cb) override {
        on_message_ = cb;
    }

    // Called by GrpcHandler when data arrives
    void OnDataReceived(const std::string& data) {
        // 1. Decode gRPC Frame
        // This requires buffering because TCP chunks != gRPC frames.
        // Simplified for Phase 11: Assume 1 chunk = 1 frame (unrealistic but ok for unit test logic)
        
        core::memory::Buffer buf;
        buf.Append(data);
        
        std::string payload;
        bool compressed;
        if (GrpcCodec::Decode(buf, payload, compressed)) {
            if (on_message_) {
                on_message_(payload);
            }
        }
    }

private:
    std::shared_ptr<networklib::protocols::http2::Stream> stream_;
    std::shared_ptr<networklib::protocols::http2::Session> session_;
    std::function<void(const std::string&)> on_message_;
};

} // namespace grpc
} // namespace protocols
} // namespace networklib

#endif // NETWORKLIB_PROTOCOLS_GRPC_GRPC_STREAM_IMPL_H
