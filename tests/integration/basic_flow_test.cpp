#include <gtest/gtest.h>
#include "network_lib.h"
#include "stream_envelope.pb.h"
#include <fstream> // Added
#include <thread>
#include <chrono>

using namespace networklib;

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup Server Config
        std::ofstream out("int_server_config.yaml");
        out << "server:\n"
            << "  protocol: tcp\n" // Use TCP for simple integration
            << "  port: 8085\n"
            << "logging:\n"
            << "  level: error\n"; // Keep logs quiet
        out.close();

        // Setup Client Config
        std::ofstream out_c("int_client_config.yaml");
        out_c << "client:\n"
              << "  mode: tcp\n"
              << "  connection:\n"
              << "    host: 127.0.0.1\n"
              << "    port: 8085\n";
        out_c.close();
    }
};

TEST_F(IntegrationTest, ConnectAndSend) {
    auto server = NetworkLib::CreateServer("int_server_config.yaml");
    ASSERT_NE(server, nullptr);

    bool received = false;
    server->RegisterStreamHandler([&](const StreamEnvelope& req, StreamEnvelope& resp, std::shared_ptr<IStreamContext> ctx) {
        received = true;
        // Echo back
        resp = req;
    });

    std::thread server_thread([&]() {
        server->Start();
        server->Wait();
    });

    // Give server time to start
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    auto client = NetworkLib::CreateClient("int_client_config.yaml");
    ASSERT_NE(client, nullptr);

    EXPECT_TRUE(client->Connect());

    StreamEnvelope env;
    env.mutable_payload()->set_data("Hello Integration");

    client->Send(env);

    // Wait for processing
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    EXPECT_TRUE(received);

    server->Stop();
    server_thread.join();
}
