#include "network/core/event/udp_socket.h"
#include "network/constants/errors.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

namespace networklib {
namespace core {
namespace event {

utils::Result<int> UdpSocket::CreateNonBlocking() {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        return utils::Result<int>::Failure(constants::errors::kInternal, "Failed to create UDP socket");
    }
    
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags != -1) {
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }
    
    // Reuse addr usually good for restart
    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    return utils::Result<int>::Success(fd);
}

utils::Result<void> UdpSocket::Bind(int fd, int port) {
    struct sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        return utils::Result<void>::Failure(constants::errors::kConnectionFailed, "UDP Bind failed");
    }
    return utils::Result<void>::Success();
}

utils::Result<ssize_t> UdpSocket::SendTo(int fd, const char* data, size_t len, const std::string& ip, int port) {
    struct sockaddr_in dest_addr;
    std::memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &dest_addr.sin_addr) <= 0) {
        return utils::Result<ssize_t>::Failure(constants::errors::kConfigError, "Invalid IP address");
    }

    ssize_t sent = sendto(fd, data, len, 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
    if (sent < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return utils::Result<ssize_t>::Success(0);
        }
        return utils::Result<ssize_t>::Failure(constants::errors::kInternal, "UDP sendto failed");
    }
    return utils::Result<ssize_t>::Success(sent);
}

utils::Result<UdpSocket::RecvResult> UdpSocket::RecvFrom(int fd, char* buf, size_t len) {
    struct sockaddr_in src_addr;
    socklen_t addr_len = sizeof(src_addr);
    
    ssize_t n = recvfrom(fd, buf, len, 0, (struct sockaddr*)&src_addr, &addr_len);
    
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return utils::Result<RecvResult>::Success({0, "", 0});
        }
        return utils::Result<RecvResult>::Failure(constants::errors::kInternal, "UDP recvfrom failed");
    }
    
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(src_addr.sin_addr), ip_str, INET_ADDRSTRLEN);
    
    return utils::Result<RecvResult>::Success({n, std::string(ip_str), ntohs(src_addr.sin_port)});
}

} // namespace event
} // namespace core
} // namespace networklib
