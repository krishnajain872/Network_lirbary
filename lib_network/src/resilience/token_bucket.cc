#include "network/core/resilience/token_bucket.h"
#include <algorithm>

namespace networklib {
namespace core {
namespace resilience {

TokenBucket::TokenBucket(double rate_per_sec, double capacity)
    : rate_(rate_per_sec), capacity_(capacity), tokens_(capacity),
      last_refill_(std::chrono::steady_clock::now()) {}

bool TokenBucket::Consume(double tokens) {
    std::lock_guard<std::mutex> lock(mutex_);
    Refill();

    if (tokens_ >= tokens) {
        tokens_ -= tokens;
        return true;
    }
    return false;
}

void TokenBucket::Refill() {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - last_refill_).count();
    
    // Add tokens based on elapsed time
    double new_tokens = (duration * rate_) / 1000000.0;
    
    if (new_tokens > 0) {
        tokens_ = std::min(capacity_, tokens_ + new_tokens);
        last_refill_ = now;
    }
}

} // namespace resilience
} // namespace core
} // namespace networklib
