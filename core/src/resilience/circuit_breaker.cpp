#include "networklib/core/resilience/circuit_breaker.h"

namespace networklib {
namespace core {
namespace resilience {

CircuitBreaker::CircuitBreaker(int failure_threshold, int reset_timeout_sec)
    : failure_threshold_(failure_threshold),
      reset_timeout_(reset_timeout_sec),
      state_(State::kClosed),
      failure_count_(0),
      success_count_(0) {}

bool CircuitBreaker::AllowRequest() {
    State s = state_.load();
    if (s == State::kClosed) return true;
    
    if (s == State::kOpen) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto now = std::chrono::steady_clock::now();
        if (now - last_failure_time_ > reset_timeout_) {
            state_ = State::kHalfOpen;
            return true; // Allow one trial request
        }
        return false;
    }

    if (s == State::kHalfOpen) {
        // Simple logic: Allow if we haven't failed yet in this probe
        // Ideally we only allow 1 concurrent request.
        return true; 
    }
    return true;
}

void CircuitBreaker::OnSuccess() {
    if (state_ == State::kHalfOpen) {
        std::lock_guard<std::mutex> lock(mutex_);
        state_ = State::kClosed;
        failure_count_ = 0;
    }
}

void CircuitBreaker::OnFailure() {
    std::lock_guard<std::mutex> lock(mutex_);
    failure_count_++;
    last_failure_time_ = std::chrono::steady_clock::now();
    
    if (state_ == State::kHalfOpen) {
        state_ = State::kOpen; // Failed probe
    } else if (failure_count_ >= failure_threshold_) {
        state_ = State::kOpen;
    }
}

CircuitBreaker::State CircuitBreaker::GetState() const {
    return state_.load();
}

} // namespace resilience
} // namespace core
} // namespace networklib
