#ifndef NETWORKLIB_NETWORK_LIB_H
#define NETWORKLIB_NETWORK_LIB_H

#include "logger/logging.h"
#include "network/detail/interfaces.h"

namespace networklib {

    class NetworkLib {
    public:
        static std::shared_ptr<IServer> CreateServer(const std::string& config_path);
        static std::shared_ptr<IClient> CreateClient(const std::string& config_path);
        static int64_t NowUs();
    };

} // namespace networklib

#endif // NETWORKLIB_NETWORK_LIB_H
