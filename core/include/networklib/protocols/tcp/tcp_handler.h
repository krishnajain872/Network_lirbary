#ifndef NETWORKLIB_PROTOCOLS_TCP_TCP_HANDLER_H
#define NETWORKLIB_PROTOCOLS_TCP_TCP_HANDLER_H

#include "networklib/protocols/protocol_handler.h"

namespace networklib {
namespace protocols {
namespace tcp {

class TcpHandler : public ProtocolHandler {
public:
    void OnConnection(const networklib::core::Connection::Ptr& conn) override;
    void OnMessage(const networklib::core::Connection::Ptr& conn) override;
};

} // namespace tcp
} // namespace protocols
} // namespace networklib

#endif // NETWORKLIB_PROTOCOLS_TCP_TCP_HANDLER_H
