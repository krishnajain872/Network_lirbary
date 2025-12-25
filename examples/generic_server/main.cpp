#include "networklib/network_lib.h"
#include "networklib/config/config.h"
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

    std::string mode = "http";
    if (argc > 1) mode = argv[1];

    // Create Config manually
    config::ServerConfig config;
    config.mode = mode;
    config.network.port = 8091;

    std::cout << "Starting C++ Generic Server (" << mode << ") on 8091..." << std::endl;

    auto server = NetworkLib::CreateServer(config);

    server->RegisterStreamHandler([](const StreamEnvelope& req, StreamEnvelope& resp, std::shared_ptr<IStreamContext> ctx) {
        std::cout << "[C++] Received Request: " << req.header().message_type() << std::endl;

        // Echo back
        resp.mutable_payload()->set_data("Hello from C++ Generic Server!");
        resp.mutable_metadata()->mutable_fields()->insert({"http_status", "200"});
    });

    shutdown_handler = [&]() {
        server->Stop();
    };

    std::cout << "Starting C++ Generic Server on 8091..." << std::endl;
    if (server->Start()) {
        server->Wait();
    }

    return 0;
}
