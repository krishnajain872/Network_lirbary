#ifndef NETWORKLIB_CORE_RESILIENCE_TOKEN_BUCKET_H
#define NETWORKLIB_CORE_RESILIENCE_TOKEN_BUCKET_H

#include <chrono>
#include <mutex>
#include <atomic>

namespace networklib {
namespace core {
namespace resilience {

/**
 * @brief Thread-safe Token Bucket for Rate Limiting.
 */
class TokenBucket {
public:
    /**
     * @param rate_per_sec Tokens added per second.
     * @param capacity Maximum burst size.
     */
    TokenBucket(double rate_per_sec, double capacity);

    /**
     * @brief Attempts to consume tokens.
     * @param tokens Number of tokens to consume (default 1).
     * @return true if allowed, false if rate limited.
     */
    bool Consume(double tokens = 1.0);

private:
    void Refill();

    const double rate_;
    const double capacity_;
    double tokens_;
    std::chrono::steady_clock::time_point last_refill_;
    std::mutex mutex_;
};

} // namespace resilience
} // namespace core
} // namespace networklib

#endif // NETWORKLIB_CORE_RESILIENCE_TOKEN_BUCKET_H
