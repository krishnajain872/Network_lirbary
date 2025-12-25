#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include "networklib/protocols/grpc/grpc_codec.h"
#include "networklib/core/memory/buffer.h"

using namespace networklib::protocols::grpc;
using namespace networklib::core::memory;

void TestGrpcCodec() {
    Buffer buf;
    std::string payload = "HelloProtobuf";
    
    // Encode
    std::vector<char> encoded = GrpcCodec::Encode(payload, false);
    
    // Verify Header
    assert(encoded.size() == 5 + payload.size());
    assert(encoded[0] == 0); // Not compressed
    // Length 13 = 0x0D
    assert(encoded[4] == 0x0D); 

    // Append to buffer
    buf.Append(encoded.data(), encoded.size());

    // Decode
    std::string out_payload;
    bool out_compressed;
    bool success = GrpcCodec::Decode(buf, out_payload, out_compressed);

    assert(success);
    assert(!out_compressed);
    assert(out_payload == payload);
    assert(buf.ReadableBytes() == 0);

    std::cout << "TestGrpcCodec PASSED" << std::endl;
}

int main() {
    TestGrpcCodec();
    return 0;
}
