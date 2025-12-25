#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "networklib/core/event/reactor.h"
#include "networklib/core/event/event_loop.h"
#include "networklib/protocols/udp/udp_handler.h"

using namespace networklib;

int main() {
    int port = 9092;
    auto loop = std::make_unique<core::event::EventLoop>();
    core::event::Reactor reactor(std::move(loop));
    auto handler = std::make_shared<protocols::udp::EchoUdpHandler>();
    
    reactor.RegisterUdpServer(port, handler);
    
    std::cout << "UDP Server running on " << port << "..." << std::endl;
    
    // Client thread
    std::thread client([port]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        
        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        struct sockaddr_in serv_addr;
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);
        inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
        
        std::string msg = "Hello UDP";
        sendto(sock, msg.c_str(), msg.size(), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        
        char buffer[1024];
        socklen_t len = sizeof(serv_addr);
        ssize_t n = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&serv_addr, &len);
        
        if (n > 0) {
            std::string reply(buffer, n);
            std::cout << "Client Received: " << reply << std::endl;
            if (reply == msg) {
                std::cout << "TestUdpEcho PASSED" << std::endl;
                exit(0);
            }
        }
        std::cout << "TestUdpEcho FAILED" << std::endl;
        exit(1);
    });
    client.detach();
    
    reactor.Run();
    return 0;
}
