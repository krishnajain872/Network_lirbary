#ifndef NETWORKLIB_CORE_MEMORY_ALLOCATOR_H
#define NETWORKLIB_CORE_MEMORY_ALLOCATOR_H

#include <cstddef>
#include <memory>

namespace networklib {
namespace core {
namespace memory {

/**
 * @brief Interface for custom memory allocators.
 */
class Allocator {
public:
    virtual ~Allocator() = default;

    /**
     * @brief Allocates a block of memory.
     * @param size Size in bytes.
     * @return Pointer to allocated memory.
     */
    virtual void* Allocate(std::size_t size) = 0;

    /**
     * @brief Deallocates a block of memory.
     * @param ptr Pointer to memory.
     */
    virtual void Deallocate(void* ptr) = 0;
};

} // namespace memory
} // namespace core
} // namespace networklib

#endif // NETWORKLIB_CORE_MEMORY_ALLOCATOR_H
