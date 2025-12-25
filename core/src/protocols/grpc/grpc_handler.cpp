#include "networklib/protocols/grpc/grpc_handler.h"
#include "networklib/protocols/grpc/grpc_codec.h"
#include "networklib/protocols/grpc/service_registry.h"
#include "grpc_stream_impl.h"
#include <iostream>

namespace networklib {
namespace protocols {
namespace grpc {

void GrpcHandler::OnConnection(const core::Connection::Ptr& conn) {
    // Create HTTP/2 Session and store in context
    auto session = std::make_shared<http2::Session>(conn);
    
    // Wire up data callback
    session->SetOnStreamData([this, session](std::shared_ptr<http2::Stream> stream, const std::vector<uint8_t>& data) {
        // Find or create GrpcStream wrapper
        // For Phase 11, we create a temporary one or need a map in GrpcHandler.
        // Assuming map is complex for this snippet, let's process immediately.
        
        // Pass session to GrpcStreamImpl so it can write back
        auto grpc_stream = std::make_shared<GrpcStreamImpl>(stream, session);
        
        // Convert to string for legacy reasons or use buffer
        std::string s_data(data.begin(), data.end());
        grpc_stream->OnDataReceived(s_data);
        
        // Dispatch (Assuming path is known or fixed for this test)
        // Real logic needs to parse HEADERS frame to get :path
        ServiceRegistry::Instance().Dispatch("/test.Service/Method", grpc_stream);
    });

    conn->SetContext(session);
    
    // In a real implementation, we would send initial SETTINGS frame here
    std::cout << "gRPC Connection Established (HTTP/2 Session Created)" << std::endl;
}

void GrpcHandler::OnMessage(const core::Connection::Ptr& conn) {
    try {
        auto session = std::any_cast<std::shared_ptr<http2::Session>>(conn->GetContext());
        if (session) {
            session->OnData();
            
            // Check for completed streams/frames
            // In a real implementation, Session would callback via interface.
            // For Phase 10 demo, we assume the Session pushes data to a stream buffer
            // And we poll it or (better) the Stream notifies us.
            // But Session logic in Phase 9 just logged to stdout.
            // We need to upgrade Session/Stream to actually hold data we can consume.
        }
    } catch (const std::bad_any_cast& e) {
        std::cerr << "GrpcHandler: Context cast failed: " << e.what() << std::endl;
    }
}

} // namespace grpc
} // namespace protocols
} // namespace networklib
