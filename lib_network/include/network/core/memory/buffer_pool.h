#ifndef NETWORKLIB_CORE_MEMORY_BUFFER_POOL_H
#define NETWORKLIB_CORE_MEMORY_BUFFER_POOL_H

#include <vector>
#include <mutex>
#include <cstddef>

namespace networklib {
namespace core {
namespace memory {

/**
 * @brief Pool for raw byte buffers (char arrays).
 * 
 * Used by the Buffer class to avoid reallocating internal vectors.
 */
class BufferPool {
public:
    static BufferPool& Instance() {
        static BufferPool instance;
        return instance;
    }

    /**
     * @brief Acquires a buffer of at least the requested size.
     * Note: Current implementation is simple fixed-size pooling.
     */
    std::vector<char> Acquire(std::size_t size);

    /**
     * @brief Returns a buffer to the pool.
     */
    void Release(std::vector<char>&& buffer);

private:
    BufferPool() = default;
    
    // Simple pool of 64KB buffers
    std::vector<std::vector<char>> pool_;
    std::mutex mutex_;
};

} // namespace memory
} // namespace core
} // namespace networklib

#endif // NETWORKLIB_CORE_MEMORY_BUFFER_POOL_H
