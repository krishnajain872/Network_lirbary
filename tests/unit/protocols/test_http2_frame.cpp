#include <iostream>
#include <cassert>
#include <vector>
#include <cstring>
#include "networklib/protocols/http2/frame_parser.h"
#include "networklib/protocols/http2/frame.h"
#include "networklib/core/memory/buffer.h"

using namespace networklib::protocols::http2;
using namespace networklib::core::memory;

void TestFrameParser() {
    FrameParser parser;
    Buffer buf;

    // Construct a DATA frame manually
    // Length: 5 (0x000005)
    // Type: DATA (0x00)
    // Flags: END_STREAM (0x01)
    // Stream ID: 1 (0x00000001)
    // Payload: "Hello"
    
    std::vector<uint8_t> raw_frame = {
        0x00, 0x00, 0x05,       // Length
        0x00,                   // Type
        0x01,                   // Flags
        0x00, 0x00, 0x00, 0x01, // Stream ID
        'H', 'e', 'l', 'l', 'o' // Payload
    };

    buf.Append(reinterpret_cast<const char*>(raw_frame.data()), raw_frame.size());

    Frame frame;
    bool success = parser.Parse(buf, frame);
    
    assert(success);
    assert(frame.header.length == 5);
    assert(frame.header.type == FrameType::DATA);
    assert(frame.header.flags == 0x01);
    assert(frame.header.stream_id == 1);
    assert(frame.payload.size() == 5);
    std::string payload_str(frame.payload.begin(), frame.payload.end());
    assert(payload_str == "Hello");

    std::cout << "TestFrameParser PASSED" << std::endl;
}

int main() {
    TestFrameParser();
    return 0;
}
