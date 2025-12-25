#ifndef NETWORKLIB_PROTOCOLS_PROTOCOL_HANDLER_H
#define NETWORKLIB_PROTOCOLS_PROTOCOL_HANDLER_H

#include <memory>
#include <string>
#include "networklib/core/connection.h"

namespace networklib {
namespace protocols {

/**
 * @brief Abstract base class for all protocol handlers.
 * 
 * Defines the interface for handling connection events and data processing.
 * Concrete implementations (TcpHandler, HttpHandler) implement protocol-specific logic.
 */
class ProtocolHandler {
public:
    virtual ~ProtocolHandler() = default;
    
    /**
     * @brief Called when a new connection is accepted.
     * 
     * @param conn The new connection.
     */
    virtual void OnConnection(const networklib::core::Connection::Ptr& conn) = 0;
    
    /**
     * @brief Called when data is available on the socket.
     * 
     * @param conn The connection with available data.
     */
    virtual void OnMessage(const networklib::core::Connection::Ptr& conn) = 0;
    
    // Factory method type
    using Ptr = std::shared_ptr<ProtocolHandler>;
};

class ProtocolFactory {
public:
    static std::shared_ptr<ProtocolHandler> Create(const std::string& mode);
};

} // namespace protocols
} // namespace networklib

#endif // NETWORKLIB_PROTOCOLS_PROTOCOL_HANDLER_H
