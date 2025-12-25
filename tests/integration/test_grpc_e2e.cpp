#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "networklib/core/event/reactor.h"
#include "networklib/protocols/grpc/grpc_handler.h"
#include "networklib/protocols/grpc/service_registry.h"
#include "networklib/protocols/grpc/grpc_stream.h"
#include "networklib/core/event/event_loop.h"
#include "networklib/core/connection.h"
#include "networklib/protocols/grpc/grpc_codec.h"

using namespace networklib;

void TestService(std::shared_ptr<protocols::grpc::GrpcStream> stream) {
    stream->SetOnMessage([stream](const std::string& msg) {
        std::cout << "[Server] Received: " << msg << std::endl;
        stream->Write("Echo: " + msg);
        // stream->Finish(0); // Keep open for now
    });
}

void RunClient(int port, std::atomic<bool>& success) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Wait for server

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "[Client] Connection failed" << std::endl;
        return;
    }

    // 1. Send HTTP/2 Connection Preface (Client Magic)
    // 24 bytes: PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n
    std::string preface = "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n";
    send(sock, preface.c_str(), preface.size(), 0);

    // 2. Send Mocked DATA Frame (skipping SETTINGS/HEADERS for this raw test)
    // We send stream ID 1, DATA frame with "Hello"
    // Encode gRPC payload
    std::string payload = "Hello";
    std::vector<char> grpc_frame = protocols::grpc::GrpcCodec::Encode(payload);
    
    // Encode HTTP/2 Frame Header
    // Length: grpc_frame.size()
    // Type: 0x00 (DATA)
    // Flags: 0x00
    // Stream ID: 1
    uint32_t len = grpc_frame.size();
    uint8_t header[9];
    header[0] = (len >> 16) & 0xFF;
    header[1] = (len >> 8) & 0xFF;
    header[2] = len & 0xFF;
    header[3] = 0x00;
    header[4] = 0x00;
    header[5] = 0;
    header[6] = 0;
    header[7] = 0;
    header[8] = 1;

    send(sock, header, 9, 0);
    send(sock, grpc_frame.data(), len, 0);

    // 3. Read Response
    char buffer[1024];
    ssize_t n = recv(sock, buffer, sizeof(buffer), 0);
    if (n > 0) {
        // Parse HTTP/2 Frame (Simplified)
        // Assume first 9 bytes are header
        if (n > 9) {
            uint32_t rlen = (static_cast<uint8_t>(buffer[0]) << 16) | (static_cast<uint8_t>(buffer[1]) << 8) | static_cast<uint8_t>(buffer[2]);
            uint8_t rtype = buffer[3];
            
            if (rtype == 0x00) { // DATA
                // Payload is at buffer[9]
                core::memory::Buffer buf;
                buf.Append(buffer + 9, rlen);
                
                std::string out_payload;
                bool compressed;
                if (protocols::grpc::GrpcCodec::Decode(buf, out_payload, compressed)) {
                    std::cout << "[Client] Response: " << out_payload << std::endl;
                    if (out_payload == "Echo: Hello") {
                        success = true;
                    }
                }
            }
        }
    }
    
    close(sock);
}

int main() {
    // Register Service
    protocols::grpc::ServiceRegistry::Instance().RegisterMethod("/test.Service/Method", TestService);

    auto loop = std::make_unique<core::event::EventLoop>();
    core::event::Reactor reactor(std::move(loop));
    auto handler = std::make_shared<protocols::grpc::GrpcHandler>();
    
    // Server on 50052 to avoid conflict
    int port = 50052;
    reactor.RegisterServer(port, handler);
    
    std::cout << "gRPC Server running on " << port << "..." << std::endl;
    
    std::atomic<bool> success{false};
    std::thread client_thread(RunClient, port, std::ref(success));
    
    // Run loop for a bit
    std::thread server_thread([&]() {
        reactor.Run();
    });
    
    client_thread.join();
    
    // Stop server (hard exit for test)
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // Ideally we stop reactor via signal or method, but exit(0) works for CI
    
    if (success) {
        std::cout << "TestGrpcE2E PASSED" << std::endl;
        exit(0);
    } else {
        std::cerr << "TestGrpcE2E FAILED" << std::endl;
        exit(1);
    }
}
