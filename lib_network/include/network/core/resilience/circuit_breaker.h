#ifndef NETWORKLIB_CORE_RESILIENCE_CIRCUIT_BREAKER_H
#define NETWORKLIB_CORE_RESILIENCE_CIRCUIT_BREAKER_H

#include <chrono>
#include <mutex>
#include <atomic>

namespace networklib {
namespace core {
namespace resilience {

class CircuitBreaker {
public:
    enum class State { kClosed, kOpen, kHalfOpen };

    /**
     * @param failure_threshold Count of failures before opening.
     * @param reset_timeout_sec Seconds to wait before Half-Open.
     */
    CircuitBreaker(int failure_threshold, int reset_timeout_sec);

    /**
     * @brief Checks if request is allowed.
     */
    bool AllowRequest();

    /**
     * @brief Records a success (may close the circuit).
     */
    void OnSuccess();

    /**
     * @brief Records a failure (may open the circuit).
     */
    void OnFailure();

    State GetState() const;

private:
    const int failure_threshold_;
    const std::chrono::seconds reset_timeout_;
    
    std::atomic<State> state_;
    std::atomic<int> failure_count_;
    std::atomic<int> success_count_;
    
    std::chrono::steady_clock::time_point last_failure_time_;
    std::mutex mutex_;
};

} // namespace resilience
} // namespace core
} // namespace networklib

#endif // NETWORKLIB_CORE_RESILIENCE_CIRCUIT_BREAKER_H
