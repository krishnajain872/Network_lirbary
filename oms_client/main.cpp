#include "networklib/network_lib.h"
#include "stream_envelope.pb.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <string>

using namespace networklib;

void RunOrderPlacementScenario(std::shared_ptr<IClient> client);
void RunMarketDataConsumerScenario(std::shared_ptr<IClient> client);
void RunBenchmarkScenario(std::shared_ptr<IClient> client);

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: oms_client <config_path> [scenario]" << std::endl;
        std::cerr << "Scenarios: order_placement, market_data, benchmark" << std::endl;
        return 1;
    }

    std::string config_path = argv[1];
    std::string scenario = (argc > 2) ? argv[2] : "interactive";

    try {
        auto client = NetworkLib::CreateClient(config_path);
        if (!client) {
            std::cerr << "Failed to create client." << std::endl;
            return 1;
        }

        LOG_INFO("Connecting to server...");
        if (!client->Connect()) {
            LOG_ERROR("Failed to connect.");
            return 1;
        }
        LOG_INFO("Connected!");

        client->RegisterMessageHandler([](const StreamEnvelope& msg) {
             (void)msg;
             LOG_INFO("Received message from server");
        });

        if (scenario == "order_placement") {
            RunOrderPlacementScenario(client);
        } else if (scenario == "market_data") {
            RunMarketDataConsumerScenario(client);
        } else if (scenario == "benchmark") {
            RunBenchmarkScenario(client);
        } else {
            while (true) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }

        client->Disconnect();

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
