#include <gtest/gtest.h>
#include "networklib/network_lib.h"
#include "stream_envelope.pb.h"
#include <fstream>
#include <thread>
#include <chrono>

using namespace networklib;

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::ofstream out("int_server_config.yaml");
        out << "server:\n"
            << "  mode: tcp\n"
            << "  network:\n"
            << "    port: 8085\n"
            << "    protocol: tcp\n"
            << "logging:\n"
            << "  level: debug\n"
            << "  sinks:\n"
            << "    - type: console\n";
        out.close();

        std::ofstream out_c("int_client_config.yaml");
        out_c << "client:\n"
              << "  mode: tcp\n"
              << "  connection:\n"
              << "    host: 127.0.0.1\n"
              << "    port: 8085\n"
              << "logging:\n"
              << "  level: debug\n"
              << "  sinks:\n"
              << "    - type: console\n";
        out_c.close();
    }
};

TEST_F(IntegrationTest, ConnectAndSend) {
    auto server = NetworkLib::CreateServer("int_server_config.yaml");
    ASSERT_NE(server, nullptr);

    bool received = false;
    server->RegisterStreamHandler([&](const StreamEnvelope& req, StreamEnvelope& resp, std::shared_ptr<IStreamContext> ctx) {
        (void)resp; (void)ctx;
        received = true;
        if (req.has_payload() && req.payload().data() == "Hello Integration") {
            // Success
        }
    });

    server->Start();

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    auto client = NetworkLib::CreateClient("int_client_config.yaml");
    ASSERT_NE(client, nullptr);

    EXPECT_TRUE(client->Connect());

    // Wait for connection to be established (async connect)
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    StreamEnvelope env;
    env.mutable_payload()->set_data("Hello Integration");

    bool sent = client->Send(env);
    EXPECT_TRUE(sent);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    EXPECT_TRUE(received);

    server->Stop();
}
