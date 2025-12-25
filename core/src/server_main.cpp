#include "networklib/network_lib.h"
#include "networklib/logger.h"
#include <iostream>
#include <csignal>

using namespace networklib;

// Global shutdown hook
std::function<void()> shutdown_handler;
void signal_handler(int) {
    if (shutdown_handler) shutdown_handler();
}

int main(int argc, char** argv) {
    // Initialize logging
    NetworkLib::InitializeLogger("appname=NetworkServer;console=true;severity=Info");

    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    std::string config_path = "config/examples/basic_tcp.yaml";
    if (argc > 1) {
        config_path = argv[1];
    }

    try {
        networklib::logging::Logger::Log(networklib::logging::LogLevel::Info, __FILE__, __LINE__, __FUNCTION__, "Starting server from %s...", config_path.c_str());
        
        auto server = NetworkLib::CreateServer(config_path);
        
        shutdown_handler = [&]() {
            networklib::logging::Logger::Log(networklib::logging::LogLevel::Info, __FILE__, __LINE__, __FUNCTION__, "Shutting down...");
            server->Stop();
        };

        if (server->Start()) {
            networklib::logging::Logger::Log(networklib::logging::LogLevel::Info, __FILE__, __LINE__, __FUNCTION__, "Server started. Press Ctrl+C to stop.");
            server->Wait();
        } else {
            networklib::logging::Logger::Log(networklib::logging::LogLevel::Error, __FILE__, __LINE__, __FUNCTION__, "Failed to start server.");
            return 1;
        }

    } catch (const std::exception& e) {
        networklib::logging::Logger::Log(networklib::logging::LogLevel::Fatal, __FILE__, __LINE__, __FUNCTION__, "Fatal error: %s", e.what());
        return 1;
    }

    networklib::logging::Logger::Deinitialize();
    return 0;
}
