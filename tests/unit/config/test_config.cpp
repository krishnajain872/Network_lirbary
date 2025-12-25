#include <iostream>
#include <cassert>
#include "networklib/config/config.h"
#include <fstream>

using namespace networklib::config;

void TestConfigParser() {
    std::ofstream temp("temp_test.yaml");
    temp << "server:\n  mode: \"tcp\"\n  name: \"unit-test\"\n  version: \"0.1\"\n  network:\n    host: \"127.0.0.1\"\n    port: 9000\n    protocol: \"tcp\"\n";
    temp.close();

    ServerConfig config = ConfigParser::Parse("temp_test.yaml");
    
    assert(config.mode == "tcp");
    assert(config.name == "unit-test");
    assert(config.network.port == 9000);
    
    std::cout << "TestConfigParser PASSED" << std::endl;
}

int main() {
    TestConfigParser();
    return 0;
}
