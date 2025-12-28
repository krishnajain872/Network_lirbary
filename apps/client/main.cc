#include "network/network_lib.h"
#include "stream_envelope.pb.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>

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
        auto logger = networklib::logging::Logger::Initialize("appname=oms-client;level=Info;output=file;file_path=oms_client.log");
        auto client = NetworkLib::CreateClient(config_path);
        if (!client) {
            std::cerr << "Failed to create client." << std::endl;
            return 1;
        }

        LOG_INFO("Connecting to server...");
        // #region agent log
        { std::ofstream log(".cursor/debug.log", std::ios::app); auto t = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); log << "{\"sessionId\":\"debug-session\",\"runId\":\"run1\",\"hypothesisId\":\"A\",\"location\":\"oms_client/main.cpp:32\",\"message\":\"About to call client->Connect()\",\"data\":{},\"timestamp\":" << t << "}\n"; }
        // #endregion
        if (!client->Connect()) {
            LOG_ERROR("Failed to connect.");
            // #region agent log
            { std::ofstream log(".cursor/debug.log", std::ios::app); auto t = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); log << "{\"sessionId\":\"debug-session\",\"runId\":\"run1\",\"hypothesisId\":\"C\",\"location\":\"oms_client/main.cpp:34\",\"message\":\"client->Connect() returned false\",\"data\":{},\"timestamp\":" << t << "}\n"; }
            // #endregion
            return 1;
        }
        // #region agent log
        { std::ofstream log(".cursor/debug.log", std::ios::app); auto t = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); log << "{\"sessionId\":\"debug-session\",\"runId\":\"run1\",\"hypothesisId\":\"B\",\"location\":\"oms_client/main.cpp:37\",\"message\":\"client->Connect() returned true\",\"data\":{},\"timestamp\":" << t << "}\n"; }
        // #endregion
        LOG_INFO("Connected!");

        client->RegisterMessageHandler([](const StreamEnvelope& msg) {
             if (msg.has_header()) {
                 LOG_INFO("Received message type: %s", msg.header().message_type().c_str());
             } else {
                 LOG_INFO("Received message (no header)");
             }
        });

        if (scenario == "order_placement") {
            RunOrderPlacementScenario(client);
        } else if (scenario == "market_data") {
            RunMarketDataConsumerScenario(client);
        } else if (scenario == "benchmark") {
            RunBenchmarkScenario(client);
        } else {
            // Interactive or keep alive
            int count = 0;
            while (count < 5) { // Just run for 5 seconds then exit to avoid hanging
                std::this_thread::sleep_for(std::chrono::seconds(1));
                count++;
            }
        }

        client->Disconnect();

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
