#include "networklib/network_lib.h"
#include "networklib/config/config.h"
#include "networklib/logger.h"
#include "stream_envelope.pb.h"
#include <iostream>
#include <csignal>

using namespace networklib;

std::function<void()> shutdown_handler;
void signal_handler(int) {
    if (shutdown_handler) shutdown_handler();
}

int main(int argc, char** argv) {
    signal(SIGINT, signal_handler);

    NetworkLib::InitializeLogger("appname=GenericServer;console=true;severity=Info");

    // Create Config manually
    config::ServerConfig config;
    config.mode = mode;
    config.network.port = 8091;

    std::cout << "Starting C++ Generic Server (" << mode << ") on 8091..." << std::endl;

    auto server = NetworkLib::CreateServer(config);

    server->RegisterStreamHandler([](const StreamEnvelope& req, StreamEnvelope& resp) {
        logging::Logger::Log(logging::LogLevel::Info, __FILE__, __LINE__, __FUNCTION__, "[C++] Received Request: %s", req.header().message_type().c_str());

        // Echo back
        resp.mutable_payload()->set_data("Hello from C++ Generic Server!");
        resp.mutable_metadata()->mutable_fields()->insert({"http_status", "200"});
    });

    shutdown_handler = [&]() {
        server->Stop();
    };

    logging::Logger::Log(logging::LogLevel::Info, __FILE__, __LINE__, __FUNCTION__, "Starting C++ Generic Server on 8091...");
    if (server->Start()) {
        server->Wait();
    }

    logging::Logger::Deinitialize();
    return 0;
}
