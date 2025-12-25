#include "networklib/network_lib.h"
#include "networklib/config/config.h"
#include "networklib/logger.h"
#include "stream_envelope.pb.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace networklib;

int main(int argc, char** argv) {
    NetworkLib::InitializeLogger("appname=GenericClient;console=true;severity=Info");

    std::string mode = "http";
    if (argc > 1) mode = argv[1];

    logging::Logger::Log(logging::LogLevel::Info, __FILE__, __LINE__, __FUNCTION__, "Creating Client in %s mode...", mode.c_str());

    // Create Config manually
    // In real world, use CreateClient(path)
    // But since ConfigParser::ParseClient isn't fully robust for manual file creation here,
    // we assume we can construct ClientConfig (but it is private in Client constructor).
    // Wait, ClientConfig is public in config.h.
    // Client constructor takes ClientConfig.
    // But NetworkLib::CreateClient(string) is what we exposed.
    // I should create a temp yaml file.

    std::string config_content =
        "client:\n"
        "  mode: " + mode + "\n"
        "  connection:\n"
        "    host: 127.0.0.1\n"
        "    port: 8091\n";

    // Write to file
    FILE* f = fopen("client_temp.yaml", "w");
    fputs(config_content.c_str(), f);
    fclose(f);

    auto client = NetworkLib::CreateClient("client_temp.yaml");

    if (client->Connect()) {
        logging::Logger::Log(logging::LogLevel::Info, __FILE__, __LINE__, __FUNCTION__, "Connected!");

        StreamEnvelope req;
        req.mutable_header()->set_message_type("/hello"); // For HTTP: "POST /hello"
        req.mutable_payload()->set_data("Message from Client");

        client->Send(req);
        logging::Logger::Log(logging::LogLevel::Info, __FILE__, __LINE__, __FUNCTION__, "Sent message.");

        // Wait a bit
        std::this_thread::sleep_for(std::chrono::seconds(1));
        client->Disconnect();
    } else {
        logging::Logger::Log(logging::LogLevel::Error, __FILE__, __LINE__, __FUNCTION__, "Failed to connect.");
    }

    logging::Logger::Deinitialize();
    return 0;
}
