#ifndef NETWORKLIB_PROTOCOLS_QUIC_QUIC_HANDLER_H
#define NETWORKLIB_PROTOCOLS_QUIC_QUIC_HANDLER_H

#include "networklib/protocols/protocol_handler.h"
#include <iostream>
#include <vector>

namespace networklib {
namespace protocols {
namespace quic {

class QuicHandler : public ProtocolHandler {
public:
    void OnConnection(const networklib::core::Connection::Ptr& conn) override;
    void OnMessage(const networklib::core::Connection::Ptr& conn) override;
};

} // namespace quic
} // namespace protocols
} // namespace networklib

#endif // NETWORKLIB_PROTOCOLS_QUIC_QUIC_HANDLER_H
