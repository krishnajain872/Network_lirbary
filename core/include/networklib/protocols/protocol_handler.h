#ifndef NETWORKLIB_PROTOCOLS_PROTOCOL_HANDLER_H
#define NETWORKLIB_PROTOCOLS_PROTOCOL_HANDLER_H

#include <memory>
#include <string>
#include <functional>
#include "networklib/core/connection.h"
#include "networklib/network_lib.h" // For IStreamContext

// Forward decl
namespace networklib { class StreamEnvelope; }

namespace networklib {
namespace protocols {

/**
 * @brief Abstract base class for all protocol handlers.
 */
class ProtocolHandler {
public:
    using StreamHandler = networklib::IServer::StreamHandler;

    virtual ~ProtocolHandler() = default;
    
    virtual void OnConnection(const networklib::core::Connection::Ptr& conn) = 0;
    virtual void OnMessage(const networklib::core::Connection::Ptr& conn) = 0;
    
    // Optional generic handler
    virtual void SetStreamHandler(StreamHandler handler) {
        stream_handler_ = handler;
    }

    using Ptr = std::shared_ptr<ProtocolHandler>;

protected:
    StreamHandler stream_handler_;
};

class ProtocolFactory {
public:
    static std::shared_ptr<ProtocolHandler> Create(const std::string& mode);
};

} // namespace protocols
} // namespace networklib

#endif // NETWORKLIB_PROTOCOLS_PROTOCOL_HANDLER_H
