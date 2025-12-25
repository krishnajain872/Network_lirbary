#include "networklib/core/event/socket.h"
#include "networklib/constants/errors.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>
#include <errno.h>

namespace networklib {
namespace core {
namespace event {

utils::Result<int> Socket::CreateNonBlocking() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        return utils::Result<int>::Failure(
            constants::errors::kInternal,
            "Failed to create socket"
        );
    }
    
    int opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        close(fd);
        return utils::Result<int>::Failure(
            constants::errors::kInternal,
            "setsockopt failed"
        );
    }

    auto res = SetNonBlocking(fd);
    if (!res) {
        close(fd);
        return utils::Result<int>::Failure(res.GetError().Code(), res.GetError().Message());
    }
    return utils::Result<int>::Success(fd);
}

utils::Result<void> Socket::SetNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        return utils::Result<void>::Failure(constants::errors::kInternal, "fcntl F_GETFL failed");
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        return utils::Result<void>::Failure(constants::errors::kInternal, "fcntl F_SETFL failed");
    }
    return utils::Result<void>::Success();
}

utils::Result<void> Socket::Bind(int fd, int port) {
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        return utils::Result<void>::Failure(constants::errors::kConnectionFailed, "Bind failed");
    }
    return utils::Result<void>::Success();
}

utils::Result<void> Socket::Listen(int fd) {
    if (listen(fd, SOMAXCONN) < 0) {
        return utils::Result<void>::Failure(constants::errors::kInternal, "Listen failed");
    }
    return utils::Result<void>::Success();
}

utils::Result<int> Socket::Accept(int server_fd) {
    int new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if (new_socket < 0) {
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
             return utils::Result<int>::Failure(constants::errors::kUnavailable, "No pending connections");
        }
        return utils::Result<int>::Failure(constants::errors::kConnectionFailed, "Accept failed");
    }
    return utils::Result<int>::Success(new_socket);
}

} // namespace event
} // namespace core
} // namespace networklib
