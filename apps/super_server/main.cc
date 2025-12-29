#include "network/network_lib.h"
#include "logger/logging.h"
#include "stream_envelope.pb.h"
#include <iostream>
#include <vector>
#include <string>

using namespace networklib;

int main(int argc, char** argv) {
    std::string config_path = "apps/super_server/server_config.yaml";
    if (argc > 1) {
        config_path = argv[1];
    }

    try {
        // Initialize Logger
        // Note: The config parser generates a logger config string, but we can also just init simply for now
        // or parse the config file to get logging settings.
        // For simplicity, we use a default init and let the server components log to it.
        networklib::logging::Logger::Initialize("severity=info;console=true;");

        LOG_INFO("Starting Super Server...");
        LOG_INFO("Loading config from: %s", config_path.c_str());

        auto server = NetworkLib::CreateServer(config_path);
        if (!server) {
            LOG_ERROR("Failed to create server instance.");
            return 1;
        }

        // Register Raw Handler (for Raw Mode protocols)
        server->RegisterRawHandler([](const std::vector<char>& data, std::shared_ptr<IStreamContext> ctx) {
            // Echo raw data back
            std::string s_data(data.begin(), data.end());
            // LOG_INFO("Raw Echo: %zu bytes", data.size());
            // Avoid logging every packet in high load, but good for debug
            ctx->Write(s_data);
        });

        // Register Stream Handler (for Proto Mode protocols)
        server->RegisterStreamHandler([](const StreamEnvelope& req, StreamEnvelope& resp, std::shared_ptr<IStreamContext> ctx) {
            // Echo StreamEnvelope
            // LOG_INFO("Proto Echo: %s", req.header().message_type().c_str());
            if (req.has_payload()) {
                resp.mutable_payload()->set_data(req.payload().data());
            }
            if (req.has_header()) {
                resp.mutable_header()->set_message_type(req.header().message_type());
            }
        });

        if (server->Start()) {
            LOG_INFO("Super Server is running. Press Ctrl+C to stop.");
            server->Wait();
        } else {
            LOG_ERROR("Failed to start server.");
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
