#ifndef NETWORKLIB_CORE_EVENT_IO_URING_POLLER_H
#define NETWORKLIB_CORE_EVENT_IO_URING_POLLER_H

#include "poller.h"
#include <liburing.h>
#include <vector>

namespace networklib {
namespace core {
namespace event {

class IoUringPoller : public Poller {
public:
    explicit IoUringPoller(int queue_size = 2048);
    ~IoUringPoller() override;

    void Add(int fd, uint32_t events, void* data) override;
    void Modify(int fd, uint32_t events, void* data) override;
    void Remove(int fd) override;
    int Poll(std::vector<Event>& active_events, int timeout_ms) override;

private:
    struct io_uring ring_;
    int queue_size_;
    
    // Helper to submit a poll request
    void SubmitPoll(int fd, uint32_t events, void* data, int op);
};

} // namespace event
} // namespace core
} // namespace networklib

#endif // NETWORKLIB_CORE_EVENT_IO_URING_POLLER_H
