#include "networklib/network_lib.h"
#include "networklib/logger.h"
#include "networklib/config/config.h"
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
        // Load config first to get logger settings
        auto config = networklib::config::ConfigParser::Parse(config_path);

        // Initialize logging with config
        std::string log_config = networklib::config::ConfigParser::GenerateLoggerConfig(config.observability.logging);
        log_config += "appname=NetworkServer;"; // Ensure appname is set
        NetworkLib::InitializeLogger(log_config.c_str());

        LOG(Info, "Starting server from %s...", config_path.c_str());
        
        // Create server (ConfigParser::Parse is called again inside CreateServer internally,
        // but we already have the config. Optimally CreateServer should accept Config object,
        // but sticking to existing API for now).
        auto server = NetworkLib::CreateServer(config_path);
        
        shutdown_handler = [&]() {
            LOG(Info, "Shutting down...");
            server->Stop();
        };

        if (server->Start()) {
            LOG(Info, "Server started. Press Ctrl+C to stop.");
            server->Wait();
        } else {
            LOG(Error, "Failed to start server.");
            return 1;
        }

    } catch (const std::exception& e) {
        LOG(Fatal, "Fatal error: %s", e.what());
        return 1;
    }

    networklib::logging::Logger::Deinitialize();
    return 0;
}
