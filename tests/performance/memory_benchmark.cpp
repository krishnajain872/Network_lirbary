#include <iostream>
#include <vector>
#include <chrono>
#include "networklib/core/memory/object_pool.h"
#include "networklib/core/memory/buffer_pool.h"

using namespace networklib::core::memory;

struct TestObject {
    int x;
    char padding[100];
};

void BenchmarkObjectPool() {
    constexpr int iterations = 1000000;
    ObjectPool<TestObject> pool(1000);
    
    auto start = std::chrono::high_resolution_clock::now();
    for(int i=0; i<iterations; ++i) {
        auto obj = pool.Acquire();
        pool.Release(obj);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    
    std::cout << "ObjectPool: " << iterations << " ops in " << diff.count() << "s (" 
              << (iterations/diff.count()) << " ops/s)" << std::endl;
}

void BenchmarkNewDelete() {
    constexpr int iterations = 1000000;
    
    auto start = std::chrono::high_resolution_clock::now();
    for(int i=0; i<iterations; ++i) {
        auto obj = std::make_shared<TestObject>();
        // destructs here
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    
    std::cout << "New/Delete: " << iterations << " ops in " << diff.count() << "s (" 
              << (iterations/diff.count()) << " ops/s)" << std::endl;
}

void BenchmarkBufferPool() {
    constexpr int iterations = 1000000;
    
    auto start = std::chrono::high_resolution_clock::now();
    for(int i=0; i<iterations; ++i) {
        auto buf = BufferPool::Instance().Acquire(1024);
        BufferPool::Instance().Release(std::move(buf));
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    
    std::cout << "BufferPool: " << iterations << " ops in " << diff.count() << "s (" 
              << (iterations/diff.count()) << " ops/s)" << std::endl;
}

int main() {
    std::cout << "--- Memory Benchmarks ---" << std::endl;
    BenchmarkObjectPool();
    BenchmarkNewDelete();
    BenchmarkBufferPool();
    return 0;
}
