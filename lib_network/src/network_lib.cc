#include "network/network_lib.h"
#include "network/core/server.h"
#include "network/core/client.h"
#include "network/config/config.h" // Corrected include
#include <chrono>

namespace networklib {

std::shared_ptr<IServer> NetworkLib::CreateServer(const std::string& config_path) {
    auto config = config::ConfigParser::Parse(config_path);
    // Initialize logger if present in config
    if (!config.observability.logging.level.empty()) {
        std::string log_conf = config::ConfigParser::GenerateLoggerConfig(config.observability.logging);
        logging::Logger::Initialize(log_conf.c_str());
    }
    return std::make_shared<core::Server>(config);
}

std::shared_ptr<IClient> NetworkLib::CreateClient(const std::string& config_path) {
    auto config = config::ConfigParser::ParseClient(config_path);
    return std::make_shared<core::Client>(config);
}

int64_t NetworkLib::NowUs() {
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
}

} // namespace networklib
