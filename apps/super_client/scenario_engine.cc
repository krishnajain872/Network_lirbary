#include "scenario_engine.h"
#include "network/core/client.h"
#include "logger/logging.h"
#include "stream_envelope.pb.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <condition_variable>

using namespace networklib;

std::vector<Scenario> ScenarioEngine::Parse(const std::string& filepath) {
    std::vector<Scenario> scenarios;
    YAML::Node config = YAML::LoadFile(filepath);

    if (config["scenarios"]) {
        for (const auto& node : config["scenarios"]) {
            Scenario s;
            s.name = node["name"].as<std::string>();
            s.type = node["type"].as<std::string>();
            s.mode = node["mode"].as<std::string>("proto");
            s.target = node["target"].as<std::string>();

            for (const auto& step_node : node["steps"]) {
                Step step;
                step.action = step_node["action"].as<std::string>();
                if (step_node["data"]) step.data = step_node["data"].as<std::string>();
                s.steps.push_back(step);
            }
            scenarios.push_back(s);
        }
    }
    return scenarios;
}

bool ScenarioEngine::Execute(const Scenario& scenario) {
    LOG_INFO("Executing Scenario: %s", scenario.name.c_str());

    config::ClientConfig client_config;
    // Map scenario type to config mode.
    // "raw" mode in config bypasses protocol handler in Client.
    // However, if we want UDP Raw, we need to know it's UDP.
    // The Client class currently uses config_.mode to decide ProtocolFactory::Create(mode).
    // If mode is "raw", it might default to TCP or behave strictly as raw TCP.
    // To support UDP Raw, we might need a composite mode like "udp_raw" or pass type separately.
    // For now, adhering to the library's apparent "raw" means TCP Raw, while "udp" means UDP.
    // But we need to support UDP Raw.
    // Workaround: We will use scenario.type for the connection setup if possible.
    // Current Client implementation uses `config.mode` for everything.
    // Let's trust that "udp" mode can support RawHandler if we register it?
    // Checking Client::Connect -> ProtocolFactory::Create(mode).
    // If we pass "udp", it creates UdpClientProtocol.
    // If we pass "raw", it might fail ProtocolFactory if not registered, OR handled specially.
    // Let's assume:
    // - TCP Raw -> mode="raw"
    // - UDP Raw -> mode="udp" + RegisterRawMessageHandler

    if (scenario.type == "udp") {
        client_config.mode = "udp";
    } else {
        // TCP or HTTP
        client_config.mode = (scenario.mode == "raw") ? "raw" : "tcp";
    }

    size_t colon = scenario.target.find(':');
    if (colon != std::string::npos) {
        client_config.network.host = scenario.target.substr(0, colon);
        client_config.network.port = std::stoi(scenario.target.substr(colon + 1));
    }

    auto client = std::make_shared<core::Client>(client_config);

    std::atomic<bool> message_received{false};
    std::string received_data;
    std::mutex mtx;
    std::condition_variable cv;

    // Register handlers
    if (scenario.mode == "raw") {
        client->RegisterRawMessageHandler([&](const std::string& data) {
            std::unique_lock<std::mutex> lock(mtx);
            received_data = data;
            message_received = true;
            cv.notify_one();
            LOG_INFO("Client Received Raw: %s", data.c_str());
        });
    } else {
        client->RegisterMessageHandler([&](const StreamEnvelope& env) {
             std::unique_lock<std::mutex> lock(mtx);
             if (env.has_payload()) received_data = env.payload().data();
             message_received = true;
             cv.notify_one();
             LOG_INFO("Client Received Proto Payload: %s", received_data.c_str());
        });
    }

    for (const auto& step : scenario.steps) {
        if (step.action == "connect") {
            LOG_INFO("Action: Connect to %s", scenario.target.c_str());
            if (!client->Connect()) {
                LOG_ERROR("Failed to connect");
                return false;
            }
        } else if (step.action == "send") {
            LOG_INFO("Action: Send '%s'", step.data.c_str());
            if (scenario.mode == "raw") {
                client->Send(step.data);
            } else {
                StreamEnvelope env;
                env.mutable_header()->set_message_type("test_msg");
                env.mutable_payload()->set_data(step.data);
                client->Send(env);
            }
        } else if (step.action == "expect") {
            LOG_INFO("Action: Expect '%s'", step.data.c_str());
            std::unique_lock<std::mutex> lock(mtx);
            if (!cv.wait_for(lock, std::chrono::seconds(5), [&]{ return message_received.load(); })) {
                LOG_ERROR("Timeout waiting for data");
                return false;
            }
            if (received_data.find(step.data) == std::string::npos) {
                // Relaxed match for HTTP or partial packets
                LOG_ERROR("Mismatch! Expected '%s', got '%s'", step.data.c_str(), received_data.c_str());
                return false;
            }
            message_received = false; // Reset
            LOG_INFO("Match confirmed.");
        } else if (step.action == "disconnect") {
             LOG_INFO("Action: Disconnect");
             client->Disconnect();
        }
    }

    return true;
}
