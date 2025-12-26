#include "networklib/protocols/grpc/grpc_handler.h"
#include "networklib/protocols/grpc/grpc_codec.h"
#include "networklib/protocols/grpc/service_registry.h"
#include "stream_envelope.pb.h"
#include "grpc_stream_impl.h"
#include "networklib/network_lib.h"
#include <iostream>

namespace networklib {
namespace protocols {
namespace grpc {

class GrpcStreamContext : public networklib::IStreamContext {
public:
    GrpcStreamContext(std::shared_ptr<http2::Session> session, uint32_t stream_id)
        : session_(session), stream_id_(stream_id) {}

    void Write(const StreamEnvelope& msg) override {
        if (msg.has_payload()) {
             std::vector<char> encoded = GrpcCodec::Encode(msg.payload().data());
             std::string s_encoded(encoded.begin(), encoded.end());
             session_->SendStreamData(stream_id_, s_encoded, false); // Keep open?
        }
    }

    void Close() override {
        // Send end stream frame
        session_->SendStreamData(stream_id_, "", true);
    }

private:
    std::shared_ptr<http2::Session> session_;
    uint32_t stream_id_;
};

void GrpcHandler::OnConnection(const core::Connection::Ptr& conn) {
    auto session = std::make_shared<http2::Session>(conn);
    
    session->SetOnStreamData([this, session](std::shared_ptr<http2::Stream> stream, const std::vector<uint8_t>& data) {
        if (stream_handler_) {
             core::memory::Buffer buf;
             buf.Append(reinterpret_cast<const char*>(data.data()), data.size());

             std::string payload;
             bool compressed;
             if (GrpcCodec::Decode(buf, payload, compressed)) {
                 networklib::StreamEnvelope req;
                 req.mutable_header()->set_message_type("/test.Service/Method");
                 req.mutable_payload()->set_data(payload);

                 networklib::StreamEnvelope resp;
                 auto ctx = std::make_shared<GrpcStreamContext>(session, stream->Id());

                 try {
                     stream_handler_(req, resp, ctx);

                     if (resp.has_payload()) {
                         ctx->Write(resp);
                         ctx->Close(); // End stream for unary
                     }
                 } catch (const std::exception& e) {
                     logging::Logger::Log(logging::LogLevel::Error, __FILE__, __LINE__, __FUNCTION__, "StreamHandler error: %s", e.what());
                     // Send error status?
                 }
             }
        } else {
            auto grpc_stream = std::make_shared<GrpcStreamImpl>(stream, session);
            std::string s_data(data.begin(), data.end());
            grpc_stream->OnDataReceived(s_data);
            ServiceRegistry::Instance().Dispatch("/test.Service/Method", grpc_stream);
        }
    });

    conn->SetContext(session);
    logging::Logger::Log(logging::LogLevel::Info, __FILE__, __LINE__, __FUNCTION__, "gRPC Connection Established");
}

void GrpcHandler::OnMessage(const core::Connection::Ptr& conn) {
    try {
        auto session = std::any_cast<std::shared_ptr<http2::Session>>(conn->GetContext());
        if (session) {
            session->OnData();
        }
    } catch (const std::bad_any_cast& e) {
        logging::Logger::Log(logging::LogLevel::Error, __FILE__, __LINE__, __FUNCTION__, "GrpcHandler: Context cast failed: %s", e.what());
    }
}

} // namespace grpc
} // namespace protocols
} // namespace networklib
