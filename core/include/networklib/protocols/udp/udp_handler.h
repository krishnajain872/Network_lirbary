#ifndef NETWORKLIB_PROTOCOLS_UDP_UDP_HANDLER_H
#define NETWORKLIB_PROTOCOLS_UDP_UDP_HANDLER_H

#include <string>
#include <memory>
#include <functional>
#include "networklib/protocols/protocol_handler.h" // For StreamHandler type

namespace networklib {
namespace protocols {
namespace udp {

/**
 * @brief Interface for handling UDP datagrams.
 * 
 * Unlike TCP ProtocolHandler which manages a Connection, 
 * this handles discrete packets from arbitrary sources.
 */
class UdpHandler {
public:
    virtual ~UdpHandler() = default;
    
    /**
     * @brief Called when a datagram is received.
     * 
     * @param fd The socket file descriptor (to send response back).
     * @param data The raw data received.
     * @param len Length of data.
     * @param source_ip Source IP address.
     * @param source_port Source port.
     */
    virtual void OnPacket(int fd, const char* data, size_t len, const std::string& source_ip, int source_port) = 0;

    using StreamHandler = protocols::ProtocolHandler::StreamHandler;
    virtual void SetStreamHandler(StreamHandler handler) { stream_handler_ = handler; }

protected:
    StreamHandler stream_handler_;
};

// Simple Echo Handler for testing/default use
class EchoUdpHandler : public UdpHandler {
public:
    void OnPacket(int fd, const char* data, size_t len, const std::string& source_ip, int source_port) override;
};

} // namespace udp
} // namespace protocols
} // namespace networklib

#endif // NETWORKLIB_PROTOCOLS_UDP_UDP_HANDLER_H
