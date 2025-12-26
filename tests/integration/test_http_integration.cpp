#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <cassert>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

// Simple integration test for HTTP
// 1. Starts server (mocked via logic or separate thread - here we assume user runs it or we run a simple check)
// Actually, in integration test, we can spin up the server in a thread.

#include "networklib/core/event/reactor.h"
#include "networklib/protocols/http/http_handler.h"
#include "networklib/core/event/event_loop.h"
#include "networklib/config/config.h"
#include "networklib/logger.h"

using namespace networklib;

void RunServer(int port, bool& running) {
    auto loop = std::make_unique<core::event::EventLoop>();
    loop->Init();
    core::event::Reactor reactor(std::move(loop));
    auto handler = std::make_shared<protocols::http::HttpHandler>();
    
    reactor.RegisterServer(port, handler);
    
    // Run in a non-blocking way or separate thread?
    // Reactor::Run is blocking.
    reactor.Run();
}

void ClientThread(int port, bool& success) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Wait for server

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        success = false;
        return;
    }

    std::string req = "GET /test HTTP/1.1\r\nHost: localhost\r\n\r\n";
    send(sock, req.c_str(), req.size(), 0);

    char buffer[4096] = {0};
    ssize_t n = recv(sock, buffer, sizeof(buffer), 0);
    if (n > 0) {
        std::string response(buffer, n);
        if (response.find("HTTP/1.1 200 OK") != std::string::npos &&
            response.find("Hello from NetworkLib") != std::string::npos) {
            success = true;
        } else {
            std::cerr << "Invalid response: " << response << std::endl;
            success = false;
        }
    } else {
        success = false;
    }
    close(sock);
}

int main() {
    networklib::logging::Logger::Initialize("severity=Debug;console=true");
    int port = 8081;
    bool success = false;
    bool server_running = true;

    // Start server in thread
    std::thread server_thread([port]() {
        // We need a way to stop the reactor for clean exit, but for this simple test, detaching or killing is fine.
        // Or we use a loop with timeout.
        // For Phase 5 test, we'll just run the reactor.
        auto loop = std::make_unique<core::event::EventLoop>();
        loop->Init();
        core::event::Reactor reactor(std::move(loop));
        auto handler = std::make_shared<protocols::http::HttpHandler>();
        reactor.RegisterServer(port, handler);
        reactor.Run();
    });
    server_thread.detach();

    // Run client
    ClientThread(port, success);

    if (success) {
        std::cout << "TestHttpIntegration PASSED" << std::endl;
        return 0;
    } else {
        std::cerr << "TestHttpIntegration FAILED" << std::endl;
        return 1;
    }
}
