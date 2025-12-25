#include "networklib/network_lib.h"
#include "networklib/core/server.h"
#include "networklib/core/client.h"
#include "networklib/config/config.h"

namespace networklib {

std::shared_ptr<IServer> NetworkLib::CreateServer(const config::ServerConfig& config) {
    return std::make_shared<core::Server>(config);
}

std::shared_ptr<IServer> NetworkLib::CreateServer(const std::string& config_path) {
    auto config = config::ConfigParser::Parse(config_path);
    return std::make_shared<core::Server>(config);
}

// Client Factory
std::shared_ptr<IClient> NetworkLib::CreateClient(const std::string& config_path) {
    auto config = config::ConfigParser::ParseClient(config_path);
    return std::make_shared<core::Client>(config);
}

} // namespace networklib
