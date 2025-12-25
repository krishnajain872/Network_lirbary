#ifndef NETWORKLIB_PROTOCOLS_GRPC_GRPC_STREAM_H
#define NETWORKLIB_PROTOCOLS_GRPC_GRPC_STREAM_H

#include <string>
#include <memory>
#include <functional>
#include "networklib/utils/result.h"

namespace networklib {
namespace protocols {
namespace grpc {

/**
 * @brief Interface for gRPC Streaming (Unary, Client-Stream, Server-Stream, Bidi).
 * 
 * This abstracts the underlying HTTP/2 stream logic.
 */
class GrpcStream {
public:
    using Ptr = std::shared_ptr<GrpcStream>;

    virtual ~GrpcStream() = default;

    /**
     * @brief Writes a protobuf message to the stream.
     * @param data Serialized protobuf data.
     */
    virtual void Write(const std::string& data) = 0;

    /**
     * @brief Writes headers (metadata).
     */
    virtual void WriteHeaders(int status = 200) = 0;

    /**
     * @brief Closes the stream with a status.
     * @param status gRPC status code (0 = OK).
     */
    virtual void Finish(int status = 0) = 0;

    /**
     * @brief Sets a callback for when a message is received.
     */
    virtual void SetOnMessage(std::function<void(const std::string&)> cb) = 0;
};

} // namespace grpc
} // namespace protocols
} // namespace networklib

#endif // NETWORKLIB_PROTOCOLS_GRPC_GRPC_STREAM_H
