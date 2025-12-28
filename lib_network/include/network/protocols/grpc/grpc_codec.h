#ifndef NETWORKLIB_PROTOCOLS_GRPC_GRPC_CODEC_H
#define NETWORKLIB_PROTOCOLS_GRPC_GRPC_CODEC_H

#include <vector>
#include <cstdint>
#include <string>
#include "network/core/memory/buffer.h"
#include "network/utils/result.h"

namespace networklib {
namespace protocols {
namespace grpc {

/**
 * @brief Handles gRPC Length-Prefixed-Message framing.
 * Format: [1 byte compression flag] [4 bytes big-endian length] [payload]
 */
class GrpcCodec {
public:
    static constexpr size_t kHeaderSize = 5;

    /**
     * @brief Encodes a protobuf message into gRPC wire format.
     * 
     * @param payload Serialized protobuf data.
     * @param compressed Whether the payload is compressed.
     * @return Encoded buffer (header + payload).
     */
    static std::vector<char> Encode(const std::string& payload, bool compressed = false);

    /**
     * @brief Decodes a gRPC message from the buffer.
     * 
     * @param buf Input buffer.
     * @param out_payload Output payload string.
     * @param out_compressed Output compression flag.
     * @return true if a full message was parsed, false if more data needed.
     */
    static bool Decode(core::memory::Buffer& buf, std::string& out_payload, bool& out_compressed);
};

} // namespace grpc
} // namespace protocols
} // namespace networklib

#endif // NETWORKLIB_PROTOCOLS_GRPC_GRPC_CODEC_H
