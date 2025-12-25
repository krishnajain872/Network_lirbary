#ifndef NETWORKLIB_CORE_EVENT_SOCKET_H
#define NETWORKLIB_CORE_EVENT_SOCKET_H

#include "networklib/utils/result.h"

namespace networklib {
namespace core {
namespace event {

class Socket {
public:
    static utils::Result<int> CreateNonBlocking();
    static utils::Result<void> Bind(int fd, int port);
    static utils::Result<void> Listen(int fd);
    static utils::Result<int> Accept(int server_fd);
    static utils::Result<void> SetNonBlocking(int fd);
};

} // namespace event
} // namespace core
} // namespace networklib

#endif // NETWORKLIB_CORE_EVENT_SOCKET_H
