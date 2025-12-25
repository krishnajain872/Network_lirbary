#include "networklib/core/security/ddos_protection.h"

namespace networklib {
namespace core {
namespace security {

DdosProtection::DdosProtection(const Config& config) : config_(config) {}

bool DdosProtection::AllowConnection(const std::string& ip) {
    std::lock_guard<std::mutex> lock(mutex_);
    int& count = conn_counts_[ip];
    if (count >= config_.max_conn_per_ip) {
        return false;
    }
    count++;
    return true;
}

bool DdosProtection::AllowRequest(const std::string& ip) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto now = std::chrono::steady_clock::now();
    auto& entry = req_counts_[ip];
    
    // Reset window if 1s passed
    if (std::chrono::duration_cast<std::chrono::seconds>(now - entry.second).count() >= 1) {
        entry.first = 0;
        entry.second = now;
    }

    if (entry.first >= config_.rate_limit_per_ip) {
        return false;
    }
    
    entry.first++;
    return true;
}

} // namespace security
} // namespace core
} // namespace networklib
