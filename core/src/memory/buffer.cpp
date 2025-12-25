#include "networklib/core/memory/buffer.h"
#include "networklib/core/memory/buffer_pool.h"
#include <cstring>
#include <cassert>

namespace networklib {
namespace core {
namespace memory {

static const char kCRLF[] = "\r\n";

Buffer::Buffer(std::size_t initial_size)
    : buffer_(BufferPool::Instance().Acquire(initial_size)),
      reader_index_(0),
      writer_index_(0) {}

Buffer::~Buffer() {
    if (!buffer_.empty()) {
        BufferPool::Instance().Release(std::move(buffer_));
    }
}

Buffer::Buffer(Buffer&& other) noexcept
    : buffer_(std::move(other.buffer_)),
      reader_index_(other.reader_index_),
      writer_index_(other.writer_index_) {
    other.reader_index_ = 0;
    other.writer_index_ = 0;
}

Buffer& Buffer::operator=(Buffer&& other) noexcept {
    if (this != &other) {
        if (!buffer_.empty()) {
            BufferPool::Instance().Release(std::move(buffer_));
        }
        buffer_ = std::move(other.buffer_);
        reader_index_ = other.reader_index_;
        writer_index_ = other.writer_index_;
        other.reader_index_ = 0;
        other.writer_index_ = 0;
    }
    return *this;
}

const char* Buffer::Peek() const {
    return Begin() + reader_index_;
}

std::size_t Buffer::ReadableBytes() const {
    return writer_index_ - reader_index_;
}

std::size_t Buffer::WritableBytes() const {
    return buffer_.size() - writer_index_;
}

char* Buffer::BeginWrite() {
    return Begin() + writer_index_;
}

const char* Buffer::BeginWrite() const {
    return Begin() + writer_index_;
}

void Buffer::HasWritten(std::size_t len) {
    assert(len <= WritableBytes());
    writer_index_ += len;
}

void Buffer::Retrieve(std::size_t len) {
    assert(len <= ReadableBytes());
    if (len < ReadableBytes()) {
        reader_index_ += len;
    } else {
        RetrieveAll();
    }
}

void Buffer::RetrieveAll() {
    reader_index_ = 0;
    writer_index_ = 0;
}

void Buffer::Append(const char* data, std::size_t len) {
    EnsureWritableBytes(len);
    std::memcpy(BeginWrite(), data, len);
    HasWritten(len);
}

void Buffer::Append(const std::string& str) {
    Append(str.data(), str.size());
}

void Buffer::EnsureWritableBytes(std::size_t len) {
    if (WritableBytes() < len) {
        MakeSpace(len);
    }
    assert(WritableBytes() >= len);
}

const char* Buffer::FindCRLF() const {
    const char* crlf = std::search(Peek(), BeginWrite(), kCRLF, kCRLF + 2);
    return crlf == BeginWrite() ? nullptr : crlf;
}

char* Buffer::Begin() {
    return &*buffer_.begin();
}

const char* Buffer::Begin() const {
    return &*buffer_.begin();
}

void Buffer::MakeSpace(std::size_t len) {
    if (WritableBytes() + reader_index_ < len) {
        // Not enough space even with compaction, resize (which reallocates)
        // Ideally we would grab a larger buffer from pool and copy, but vector resize is standard.
        // For strict pooling, we would implement swap with larger pool buffer.
        buffer_.resize(writer_index_ + len);
    } else {
        // Compact: move readable data to front
        std::size_t readable = ReadableBytes();
        std::copy(Begin() + reader_index_, Begin() + writer_index_, Begin());
        reader_index_ = 0;
        writer_index_ = readable;
    }
}

} // namespace memory
} // namespace core
} // namespace networklib
