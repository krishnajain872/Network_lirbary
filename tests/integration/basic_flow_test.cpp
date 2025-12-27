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
            << "  mode: grpc\n"
            << "  network:\n"
            << "    port: 8085\n"
            << "logging:\n"
            << "  level: error\n";
        out.close();

        std::ofstream out_c("int_client_config.yaml");
        out_c << "client:\n"
              << "  mode: grpc\n"
              << "  connection:\n"
              << "    host: 127.0.0.1\n"
              << "    port: 8085\n";
        out_c.close();
    }
};

TEST_F(IntegrationTest, ConnectAndSend) {
    auto server = NetworkLib::CreateServer("int_server_config.yaml");
    ASSERT_NE(server, nullptr);

    std::atomic<bool> received{false};
    server->RegisterStreamHandler([&](const StreamEnvelope& req, StreamEnvelope& resp, std::shared_ptr<IStreamContext> ctx) {
        (void)resp; (void)ctx;
        received = true;
        if (req.has_payload() && req.payload().data() == "Hello Integration") {
            // Success
        }
    });

    EXPECT_TRUE(server->Start());

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    auto client = NetworkLib::CreateClient("int_client_config.yaml");
    ASSERT_NE(client, nullptr);

    EXPECT_TRUE(client->Connect());

    StreamEnvelope env;
    env.mutable_header()->set_message_type("TEST_MSG");
    env.mutable_payload()->set_data("Hello Integration");

    client->Send(env);

    // Wait for processing
    int retries = 0;
    while (!received && retries < 20) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        retries++;
    }

    server->Stop();

    EXPECT_TRUE(received);
}
