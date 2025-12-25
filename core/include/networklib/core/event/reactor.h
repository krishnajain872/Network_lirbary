#ifndef NETWORKLIB_CORE_EVENT_REACTOR_H
#define NETWORKLIB_CORE_EVENT_REACTOR_H

#include "networklib/core/event/event_loop.h"
#include "networklib/protocols/protocol_handler.h"
#include "networklib/core/connection.h"
#include "networklib/security/tls_context.h"
#include "networklib/core/memory/object_pool.h"
#include "networklib/core/security/ddos_protection.h"
#include <memory>
#include <map>

// Forward declaration to break circular dependency
namespace networklib {
namespace protocols {
namespace udp {
    class UdpHandler;
}
}
}

namespace networklib {
namespace core {
namespace event {

/**
 * @brief Dispatcher for network events using the Reactor pattern.
 * 
 * The Reactor waits for events on the EventLoop and dispatches them 
 * to registered ProtocolHandlers. It handles the server socket lifecycle
 * and connection acceptance.
 */
class Reactor {
public:
    /**
     * @brief Constructs a Reactor with a given EventLoop.
     * @param loop Unique pointer to the EventLoop to use.
     */
    Reactor(std::unique_ptr<EventLoop> loop);

    /**
     * @brief Registers a server to listen on a specific port.
     * 
     * @param port The TCP port to listen on.
     * @param handler The protocol handler factory/instance to use for new connections.
     * @param tls_ctx Optional TLS context for secure connections.
     * @return Result<void> Success or failure.
     */
    utils::Result<void> RegisterServer(int port, 
                                     std::shared_ptr<protocols::ProtocolHandler> handler,
                                     std::shared_ptr<networklib::security::TlsContext> tls_ctx = nullptr);

    /**
     * @brief Registers a UDP server to listen on a specific port.
     */
    utils::Result<void> RegisterUdpServer(int port, std::shared_ptr<protocols::udp::UdpHandler> handler);

    /**
     * @brief Starts the reactor loop.
     */
    void Run();

    /**
     * @brief Stops the reactor loop.
     */
    void Stop();

    /**
     * @brief Updates security configurations (e.g. DDoS protection)
     */
    void SetDdosConfig(const security::DdosProtection::Config& config);

    /**
     * @brief Get the TLS context for updates.
     */
    std::shared_ptr<networklib::security::TlsContext> GetTlsContext() { return tls_context_; }

private:
    std::unique_ptr<EventLoop> loop_;
    std::map<int, std::shared_ptr<Connection>> connections_;
    std::unique_ptr<security::DdosProtection> ddos_protection_;
    std::shared_ptr<networklib::security::TlsContext> tls_context_;
};

} // namespace event
} // namespace core
} // namespace networklib

#endif // NETWORKLIB_CORE_EVENT_REACTOR_H
