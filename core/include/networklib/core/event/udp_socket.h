#ifndef NETWORKLIB_CORE_EVENT_UDP_SOCKET_H
#define NETWORKLIB_CORE_EVENT_UDP_SOCKET_H

#include "networklib/utils/result.h"
#include <string>
#include <vector>

namespace networklib {
namespace core {
namespace event {

class UdpSocket {
public:
    static utils::Result<int> CreateNonBlocking();
    static utils::Result<void> Bind(int fd, int port);
    
    // Sends data to a specific address
    static utils::Result<ssize_t> SendTo(int fd, const char* data, size_t len, const std::string& ip, int port);
    
    // Receives data and returns the source address
    struct RecvResult {
        ssize_t bytes_read;
        std::string source_ip;
        int source_port;
    };
    static utils::Result<RecvResult> RecvFrom(int fd, char* buf, size_t len);
};

} // namespace event
} // namespace core
} // namespace networklib

#endif // NETWORKLIB_CORE_EVENT_UDP_SOCKET_H
