#include "networklib/protocols/udp/udp_handler.h"
#include "networklib/core/event/udp_socket.h"
#include <iostream>

namespace networklib {
namespace protocols {
namespace udp {

void EchoUdpHandler::OnPacket(int fd, const char* data, size_t len, const std::string& source_ip, int source_port) {
    // Simple Echo
    // std::string msg(data, len);
    // std::cout << "UDP Recv from " << source_ip << ":" << source_port << " - " << msg << std::endl;
    
    // Echo back
    core::event::UdpSocket::SendTo(fd, data, len, source_ip, source_port);
}

} // namespace udp
} // namespace protocols
} // namespace networklib
