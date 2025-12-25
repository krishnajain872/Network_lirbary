#include "networklib/network_lib.h"
#include <iostream>
#include <csignal>

using namespace networklib;

// Global shutdown hook
std::function<void()> shutdown_handler;
void signal_handler(int) {
    if (shutdown_handler) shutdown_handler();
}

int main(int argc, char** argv) {
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    std::string config_path = "config/examples/basic_tcp.yaml";
    if (argc > 1) {
        config_path = argv[1];
    }

    try {
        std::cout << "Starting server from " << config_path << "..." << std::endl;
        
        auto server = NetworkLib::CreateServer(config_path);
        
        shutdown_handler = [&]() {
            std::cout << "Shutting down..." << std::endl;
            server->Stop();
        };

        if (server->Start()) {
            std::cout << "Server started. Press Ctrl+C to stop." << std::endl;
            server->Wait();
        } else {
            std::cerr << "Failed to start server." << std::endl;
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
