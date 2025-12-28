#include "network/core/memory/buffer_pool.h"
#include "network/constants/limits.h"

namespace networklib {
namespace core {
namespace memory {

std::vector<char> BufferPool::Acquire(std::size_t size) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // If pool has items and size fits roughly (simplified)
    // For now, we only pool buffers of kMaxHeaderSize (64KB) or larger
    if (!pool_.empty()) {
        auto buf = std::move(pool_.back());
        pool_.pop_back();
        if (buf.capacity() >= size) {
            buf.clear(); // Keep capacity
            return buf;
        }
    }
    
    // Create new
    return std::vector<char>(size);
}

void BufferPool::Release(std::vector<char>&& buffer) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (pool_.size() < constants::limits::kMaxConnectionPoolSize) {
        pool_.push_back(std::move(buffer));
    }
}

} // namespace memory
} // namespace core
} // namespace networklib
