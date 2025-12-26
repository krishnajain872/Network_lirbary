#include "networklib/network_lib.h"
#include "networklib/config/config.h"
#include "networklib/core/server.h"
#include "networklib/core/client.h"
#include <cstdarg>
#include <cstdio>
#include <iostream>

namespace networklib {

    // ========================================================================
    // FACTORY IMPLEMENTATION
    // ========================================================================

    std::shared_ptr<IServer> NetworkLib::CreateServer(const std::string& config_path) {
        // 1. Parse Config
        auto config = config::ConfigParser::Parse(config_path);

        // 2. Initialize Logger if configured
        if (!config.observability.logging.level.empty()) {
            std::string log_conf = config::ConfigParser::GenerateLoggerConfig(config.observability.logging);
            logging::Logger::Initialize(log_conf.c_str());
        }

        // 3. Create Server
        auto server = std::make_shared<core::Server>(config);

        return server;
    }

    std::shared_ptr<IClient> NetworkLib::CreateClient(const std::string& config_path) {
        // 1. Parse Config
        auto config = config::ConfigParser::ParseClient(config_path);

        // 2. Create Client
        auto client = std::make_shared<core::Client>(config);
        return client;
    }

    int64_t NetworkLib::NowUs() {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
    }

} // namespace networklib
