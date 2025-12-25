#include "networklib/network_lib.h"
#include "networklib/config/config.h"
#include "stream_envelope.pb.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace networklib;

int main(int argc, char** argv) {
    std::string mode = "http";
    if (argc > 1) mode = argv[1];

    std::cout << "Creating Client in " << mode << " mode..." << std::endl;

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
        std::cout << "Connected!" << std::endl;

        StreamEnvelope req;
        req.mutable_header()->set_message_type("/hello"); // For HTTP: "POST /hello"
        req.mutable_payload()->set_data("Message from Client");

        client->Send(req);
        std::cout << "Sent message." << std::endl;

        // Wait a bit
        std::this_thread::sleep_for(std::chrono::seconds(1));
        client->Disconnect();
    } else {
        std::cerr << "Failed to connect." << std::endl;
    }

    return 0;
}
