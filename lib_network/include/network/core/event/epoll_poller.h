#ifndef NETWORKLIB_CORE_EVENT_EPOLL_POLLER_H
#define NETWORKLIB_CORE_EVENT_EPOLL_POLLER_H

#include "poller.h"
#include <vector>

namespace networklib {
namespace core {
namespace event {

class EpollPoller : public Poller {
public:
    EpollPoller();
    ~EpollPoller() override;

    void Add(int fd, uint32_t events, void* data) override;
    void Modify(int fd, uint32_t events, void* data) override;
    void Remove(int fd) override;
    int Poll(std::vector<Event>& active_events, int timeout_ms) override;

private:
    int epoll_fd_;
};

} // namespace event
} // namespace core
} // namespace networklib

#endif // NETWORKLIB_CORE_EVENT_EPOLL_POLLER_H
