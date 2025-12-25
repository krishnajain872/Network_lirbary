#include <iostream>
#include <cassert>
#include <cstring>
#include "networklib/core/memory/buffer.h"

using namespace networklib::core::memory;

void TestBuffer() {
    Buffer buf(10);
    assert(buf.ReadableBytes() == 0);
    assert(buf.WritableBytes() == 10);

    // Write
    buf.Append("12345", 5);
    assert(buf.ReadableBytes() == 5);
    assert(buf.WritableBytes() == 5);
    assert(std::string(buf.Peek(), 5) == "12345");

    // Read partial
    buf.Retrieve(2);
    assert(buf.ReadableBytes() == 3);
    assert(std::string(buf.Peek(), 3) == "345");

    // Write causing resize/compaction
    buf.Append("67890ABC", 8); // Needs 8 bytes, have 5 writable, but 2 reclaimable
    // Total used 3, needs 8 more -> 11 total. Cap 10. Will resize.
    
    assert(buf.ReadableBytes() == 11);
    assert(std::string(buf.Peek(), 11) == "34567890ABC");

    std::cout << "TestBuffer PASSED" << std::endl;
}

int main() {
    TestBuffer();
    return 0;
}
