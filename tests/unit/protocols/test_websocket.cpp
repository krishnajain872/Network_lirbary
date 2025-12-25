#include <iostream>
#include <cassert>
#include <vector>
#include <cstring>
#include "networklib/protocols/websocket/frame.h"

using namespace networklib::protocols::websocket;

void TestWebSocketFrame() {
    FrameParser parser;
    
    // Construct a Text Frame: "Hello"
    // Fin=1, Op=1, Mask=1, Len=5
    // Mask Key: 0x00 0x00 0x00 0x00 (No mask for simplicity in test construction)
    // Payload: Hello
    
    std::vector<char> raw;
    raw.push_back(0x81); // Fin | Text
    raw.push_back(0x85); // Mask | Len=5
    
    // Mask Key
    raw.push_back(0x00); raw.push_back(0x00); raw.push_back(0x00); raw.push_back(0x00);
    
    // Payload
    raw.push_back('H'); raw.push_back('e'); raw.push_back('l'); raw.push_back('l'); raw.push_back('o');
    
    Frame frame;
    size_t consumed = 0;
    bool success = parser.Parse(raw.data(), raw.size(), consumed, frame);
    
    assert(success);
    assert(frame.fin);
    assert(frame.opcode == OpCode::kText);
    assert(frame.masked);
    assert(frame.payload.size() == 5);
    std::string text(frame.payload.begin(), frame.payload.end());
    assert(text == "Hello");
    assert(consumed == raw.size());

    std::cout << "TestWebSocketFrame PASSED" << std::endl;
}

int main() {
    TestWebSocketFrame();
    return 0;
}
