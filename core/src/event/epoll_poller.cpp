#include "networklib/core/event/epoll_poller.h"
#include "networklib/logging.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>
#include <cerrno>

namespace networklib {
namespace core {
namespace event {

EpollPoller::EpollPoller() {
    epoll_fd_ = epoll_create1(0);
    if (epoll_fd_ == -1) {
        throw std::runtime_error("Failed to create epoll fd");
    }
}

EpollPoller::~EpollPoller() {
    if (epoll_fd_ != -1) {
        close(epoll_fd_);
    }
}

void EpollPoller::Add(int fd, uint32_t events, void* data) {
    struct epoll_event ev;
    ev.events = 0;
    if (events & 0x001) ev.events |= EPOLLIN;
    if (events & 0x004) ev.events |= EPOLLOUT;
    // We can also support Edge Triggered (EPOLLET) if configured
    ev.data.ptr = data;

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev) == -1) {
        throw std::runtime_error(std::string("epoll_ctl add failed: ") + std::strerror(errno));
    }
    LOG(Trace, "Epoll Add fd=%d events=%u", fd, events);
}

void EpollPoller::Modify(int fd, uint32_t events, void* data) {
    struct epoll_event ev;
    ev.events = 0;
    if (events & 0x001) ev.events |= EPOLLIN;
    if (events & 0x004) ev.events |= EPOLLOUT;
    ev.data.ptr = data;

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ev) == -1) {
        throw std::runtime_error("epoll_ctl mod failed");
    }
    LOG(Trace, "Epoll Modify fd=%d events=%u", fd, events);
}

void EpollPoller::Remove(int fd) {
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr) == -1) {
         LOG(Debug, "epoll_ctl del failed for fd=%d: %s", fd, strerror(errno));
    } else {
         LOG(Trace, "Epoll Remove fd=%d", fd);
    }
}

int EpollPoller::Poll(std::vector<Event>& active_events, int timeout_ms) {
    constexpr int kMaxEvents = 128; // Batch size
    struct epoll_event events[kMaxEvents];

    int nfds = epoll_wait(epoll_fd_, events, kMaxEvents, timeout_ms);
    if (nfds == -1) {
        if (errno == EINTR) return 0;
        return -1;
    }

    for (int i = 0; i < nfds; ++i) {
        Event ev;
        ev.data = events[i].data.ptr;
        ev.events = 0;
        if (events[i].events & EPOLLIN) ev.events |= 0x001;
        if (events[i].events & EPOLLOUT) ev.events |= 0x004;
        if (events[i].events & (EPOLLERR | EPOLLHUP)) ev.events |= 0x001; // Treat error as Read to close

        active_events.push_back(ev);
    }
    return nfds;
}

} // namespace event
} // namespace core
} // namespace networklib
