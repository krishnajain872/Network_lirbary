#ifndef NETWORKLIB_CORE_MEMORY_OBJECT_POOL_H
#define NETWORKLIB_CORE_MEMORY_OBJECT_POOL_H

#include <vector>
#include <mutex>
#include <memory>
#include <iostream>

namespace networklib {
namespace core {
namespace memory {

/**
 * @brief Thread-safe object pool for reusing objects of type T.
 * 
 * Reduces heap fragmentation and allocation overhead by recycling objects.
 */
template<typename T>
class ObjectPool {
public:
    explicit ObjectPool(std::size_t initial_size = 1000) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (std::size_t i = 0; i < initial_size; ++i) {
            pool_.push_back(std::make_shared<T>());
        }
    }

    /**
     * @brief Acquires an object from the pool.
     * Creates a new one if pool is empty.
     */
    std::shared_ptr<T> Acquire() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (pool_.empty()) {
            // Pool empty, create new (could log warning or expand)
            return std::make_shared<T>();
        }
        
        auto obj = pool_.back();
        pool_.pop_back();
        return obj;
    }

    /**
     * @brief Returns an object to the pool.
     * 
     * @param obj The object to return.
     */
    void Release(std::shared_ptr<T> obj) {
        if (!obj) return;
        std::lock_guard<std::mutex> lock(mutex_);
        // In a real implementation, we might check if pool is full or limit size
        pool_.push_back(obj);
    }

    std::size_t Size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return pool_.size();
    }

private:
    std::vector<std::shared_ptr<T>> pool_;
    mutable std::mutex mutex_;
};

} // namespace memory
} // namespace core
} // namespace networklib

#endif // NETWORKLIB_CORE_MEMORY_OBJECT_POOL_H
