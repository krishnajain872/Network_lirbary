#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <cstring>
#include <arpa/inet.h>
#include "stream_envelope.pb.h"

using namespace networklib;

void TestStreamEnvelope() {
    StreamEnvelope envelope;
    envelope.mutable_header()->set_message_type("test.message");
    envelope.mutable_header()->set_message_id("uuid-1234");
    envelope.mutable_payload()->set_data("Hello RPC");
    
    std::string serialized;
    envelope.SerializeToString(&serialized);
    
    // Deserialize
    StreamEnvelope parsed;
    bool success = parsed.ParseFromString(serialized);
    
    assert(success);
    assert(parsed.header().message_type() == "test.message");
    assert(parsed.payload().data() == "Hello RPC");
    
    std::cout << "TestStreamEnvelope PASSED" << std::endl;
}

int main() {
    TestStreamEnvelope();
    return 0;
}
