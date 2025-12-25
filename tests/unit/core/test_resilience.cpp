#include <iostream>
#include <cassert>
#include <thread>
#include "networklib/core/resilience/token_bucket.h"
#include "networklib/core/resilience/circuit_breaker.h"

using namespace networklib::core::resilience;

void TestTokenBucket() {
    // 10 tokens/sec, capacity 10
    TokenBucket bucket(10.0, 10.0);
    
    // Consume 5
    assert(bucket.Consume(5.0));
    
    // Consume 5
    assert(bucket.Consume(5.0));
    
    // Should fail
    assert(!bucket.Consume(1.0));
    
    // Wait 0.1s -> +1 token
    std::this_thread::sleep_for(std::chrono::milliseconds(110));
    
    assert(bucket.Consume(1.0));
    
    std::cout << "TestTokenBucket PASSED" << std::endl;
}

void TestCircuitBreaker() {
    // 3 failures to open, 1s reset
    CircuitBreaker cb(3, 1);
    
    assert(cb.AllowRequest());
    cb.OnFailure(); // 1
    assert(cb.AllowRequest());
    cb.OnFailure(); // 2
    assert(cb.AllowRequest());
    cb.OnFailure(); // 3 -> OPEN
    
    assert(!cb.AllowRequest());
    assert(cb.GetState() == CircuitBreaker::State::kOpen);
    
    // Wait 1.1s -> Half Open
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
    
    assert(cb.AllowRequest()); // Allowed probe
    assert(cb.GetState() == CircuitBreaker::State::kHalfOpen);
    
    cb.OnSuccess(); // Close
    assert(cb.GetState() == CircuitBreaker::State::kClosed);
    
    std::cout << "TestCircuitBreaker PASSED" << std::endl;
}

int main() {
    TestTokenBucket();
    TestCircuitBreaker();
    return 0;
}
