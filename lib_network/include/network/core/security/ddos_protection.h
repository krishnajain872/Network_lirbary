#ifndef NETWORKLIB_CORE_SECURITY_DDOS_PROTECTION_H
#define NETWORKLIB_CORE_SECURITY_DDOS_PROTECTION_H

#include <string>
#include <map>
#include <chrono>
#include <mutex>

namespace networklib {
namespace core {
namespace security {

class DdosProtection {
public:
    struct Config {
        int max_conn_per_ip = 100;
        int rate_limit_per_ip = 50; // req/sec
        bool enable_syn_cookies = true;
    };

    explicit DdosProtection(const Config& config);

    bool AllowConnection(const std::string& ip);
    bool AllowRequest(const std::string& ip);

private:
    Config config_;
    std::mutex mutex_;
    std::map<std::string, int> conn_counts_;
    std::map<std::string, std::pair<int, std::chrono::steady_clock::time_point>> req_counts_;
};

} // namespace security
} // namespace core
} // namespace networklib

#endif // NETWORKLIB_CORE_SECURITY_DDOS_PROTECTION_H
