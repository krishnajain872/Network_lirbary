#ifndef NETWORKLIB_CORE_EVENT_POLLER_H
#define NETWORKLIB_CORE_EVENT_POLLER_H

#include <vector>
#include <chrono>

namespace networklib {
namespace core {
namespace event {

struct Event {
    int fd;
    uint32_t events;
    void* data;
};

class Poller {
public:
    virtual ~Poller() = default;
    virtual void Add(int fd, uint32_t events, void* data) = 0;
    virtual void Modify(int fd, uint32_t events, void* data) = 0;
    virtual void Remove(int fd) = 0;
    virtual int Poll(std::vector<Event>& active_events, int timeout_ms) = 0;
};

} // namespace event
} // namespace core
} // namespace networklib

#endif // NETWORKLIB_CORE_EVENT_POLLER_H
