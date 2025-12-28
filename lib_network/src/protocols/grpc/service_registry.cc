#include "network/protocols/grpc/service_registry.h"
#include "network/constants/errors.h"

namespace networklib {
namespace protocols {
namespace grpc {

void ServiceRegistry::RegisterMethod(const std::string& path, StreamCallback callback) {
    methods_[path] = std::move(callback);
}

utils::Result<void> ServiceRegistry::Dispatch(const std::string& path, std::shared_ptr<GrpcStream> stream) {
    auto it = methods_.find(path);
    if (it == methods_.end()) {
        stream->Finish(constants::errors::kNotFound); // Or gRPC specific status UNIMPLEMENTED
        return utils::Result<void>::Failure(
            constants::errors::kNotFound,
            "Method not found: " + path
        );
    }
    it->second(stream);
    return utils::Result<void>::Success();
}

} // namespace grpc
} // namespace protocols
} // namespace networklib
