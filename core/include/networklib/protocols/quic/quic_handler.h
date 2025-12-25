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
    void OnConnection(std::shared_ptr<core::Connection> conn) override;
    void OnData(std::shared_ptr<core::Connection> conn, const std::vector<uint8_t>& data) override;
    void OnDisconnect(std::shared_ptr<core::Connection> conn) override;
};

} // namespace quic
} // namespace protocols
} // namespace networklib

#endif // NETWORKLIB_PROTOCOLS_QUIC_QUIC_HANDLER_H
