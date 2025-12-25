#ifndef NETWORKLIB_CORE_MEMORY_BUFFER_H
#define NETWORKLIB_CORE_MEMORY_BUFFER_H

#include <vector>
#include <string>
#include <cstddef>
#include <algorithm>

namespace networklib {
namespace core {
namespace memory {

/**
 * @brief Dynamic byte buffer for I/O operations.
 * 
 * Provides efficient appending and consuming of bytes.
 * Based on std::vector for contiguous memory storage, suitable for socket I/O.
 */
class Buffer {
public:
    explicit Buffer(std::size_t initial_size = 1024);
    ~Buffer();

    // Disable copy, enable move (important for pooling)
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer(Buffer&&) noexcept;
    Buffer& operator=(Buffer&&) noexcept;

    /**
     * @brief Returns pointer to readable data.
     */
    const char* Peek() const;

    /**
     * @brief Returns readable size.
     */
    std::size_t ReadableBytes() const;

    /**
     * @brief Returns writable size (capacity - writer_index).
     */
    std::size_t WritableBytes() const;

    /**
     * @brief Returns pointer to writable area.
     */
    char* BeginWrite();
    const char* BeginWrite() const;

    /**
     * @brief Advances writer index after writing data.
     */
    void HasWritten(std::size_t len);

    /**
     * @brief Consumes data (advances reader index).
     */
    void Retrieve(std::size_t len);

    /**
     * @brief Consumes all data.
     */
    void RetrieveAll();

    /**
     * @brief Appends data to the buffer.
     */
    void Append(const char* data, std::size_t len);
    void Append(const std::string& str);

    /**
     * @brief Ensures writable space exists, resizing if necessary.
     */
    void EnsureWritableBytes(std::size_t len);

    /**
     * @brief Helper to find CRLF (for HTTP).
     */
    const char* FindCRLF() const;

private:
    char* Begin();
    const char* Begin() const;
    void MakeSpace(std::size_t len);

    std::vector<char> buffer_;
    std::size_t reader_index_;
    std::size_t writer_index_;
};

} // namespace memory
} // namespace core
} // namespace networklib

#endif // NETWORKLIB_CORE_MEMORY_BUFFER_H
