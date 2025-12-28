#include <iostream>
#include <memory>
#include <string>
#include "network/core/connection.h"
#include "network/protocols/grpc/grpc_codec.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>

// Simple blocking gRPC client stub for testing
namespace networklib {
namespace protocols {
namespace grpc {

class GrpcClient {
public:
    GrpcClient(std::shared_ptr<core::event::EventLoop> loop) : loop_(loop) {}

    bool Connect(const std::string& host, int port) {
        // Create raw connection
        // We need a dummy FD to start, Reactor logic usually does this.
        // For client, we create socket first.
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        conn_ = std::make_shared<core::Connection>(loop_.get(), fd);
        
        auto res = conn_->Connect(host, port);
        return res.HasValue();
    }

    void SendUnary(const std::string& path, const std::string& protobuf_data) {
        if (!conn_) return;

        // 1. Send HTTP/2 Connection Preface (Client Magic)
        std::string preface = "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n";
        conn_->Send(preface);

        // 2. Send SETTINGS Frame
        // 3. Send HEADERS Frame (POST path)
        // 4. Send DATA Frame (gRPC framed protobuf)
        
        std::vector<char> grpc_frame = GrpcCodec::Encode(protobuf_data);
        // ... (Framing logic omitted for brevity in Phase 10 demo)
        // We will send raw bytes for the test to simulate a "Hello" packet
    }

private:
    std::shared_ptr<core::event::EventLoop> loop_;
    core::Connection::Ptr conn_;
};

} // namespace grpc
} // namespace protocols
} // namespace networklib
