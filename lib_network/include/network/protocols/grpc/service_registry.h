#ifndef NETWORKLIB_PROTOCOLS_GRPC_SERVICE_REGISTRY_H
#define NETWORKLIB_PROTOCOLS_GRPC_SERVICE_REGISTRY_H

#include <string>
#include <map>
#include <functional>
#include <memory>
#include "network/utils/result.h"
#include "network/protocols/grpc/grpc_stream.h"

namespace networklib {
namespace protocols {
namespace grpc {

// Streaming Callback
// The handler receives a stream interface to read from and write to.
using StreamCallback = std::function<void(std::shared_ptr<GrpcStream>)>;

class ServiceRegistry {
public:
    static ServiceRegistry& Instance() {
        static ServiceRegistry instance;
        return instance;
    }

    void RegisterMethod(const std::string& path, StreamCallback callback);
    
    // Dispatch now takes the Stream interface
    utils::Result<void> Dispatch(const std::string& path, std::shared_ptr<GrpcStream> stream);

private:
    ServiceRegistry() = default;
    std::map<std::string, StreamCallback> methods_;
};

} // namespace grpc
} // namespace protocols
} // namespace networklib

#endif // NETWORKLIB_PROTOCOLS_GRPC_SERVICE_REGISTRY_H
