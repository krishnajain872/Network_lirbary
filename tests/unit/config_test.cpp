#include <gtest/gtest.h>
#include "network_lib.h"
#include <fstream>

using namespace networklib;

TEST(ConfigParserTest, ParseServerConfig) {
    // Write temp config
    std::ofstream out("test_server_config.yaml");
    out << "server:\n"
        << "  protocol: grpc\n"
        << "  port: 9090\n"
        << "logging:\n"
        << "  level: info\n";
    out.close();

    auto server = NetworkLib::CreateServer("test_server_config.yaml");
    EXPECT_NE(server, nullptr);
}

TEST(ConfigParserTest, ParseClientConfig) {
    std::ofstream out("test_client_config.yaml");
    out << "client:\n"
        << "  mode: tcp\n"
        << "  connection:\n"
        << "    host: localhost\n"
        << "    port: 9090\n";
    out.close();

    auto client = NetworkLib::CreateClient("test_client_config.yaml");
    EXPECT_NE(client, nullptr);
}
