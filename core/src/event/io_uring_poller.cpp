#include "networklib/core/event/io_uring_poller.h"
#include "networklib/network_lib.h"
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <poll.h>

namespace networklib {
namespace core {
namespace event {

IoUringPoller::IoUringPoller(int queue_size) : queue_size_(queue_size) {
    if (io_uring_queue_init(queue_size_, &ring_, 0) < 0) {
        LOG(Fatal, "Failed to initialize io_uring: %s", strerror(errno));
        throw std::runtime_error("Failed to initialize io_uring");
    }
}

IoUringPoller::~IoUringPoller() {
    io_uring_queue_exit(&ring_);
}

void IoUringPoller::Add(int fd, uint32_t events, void* data) {
    SubmitPoll(fd, events, data, IORING_OP_POLL_ADD);
    LOG(Trace, "io_uring add fd=%d events=%u", fd, events);
}

void IoUringPoller::Modify(int fd, uint32_t events, void* data) {
    // In io_uring, typical pattern is to remove and add, or use multi-shot poll
    // For simplicity/compatibility with Poller interface, we'll implement as remove+add
    // Ideally we would use IORING_POLL_ADD_MULTI
    Remove(fd);
    Add(fd, events, data);
    LOG(Trace, "io_uring modify fd=%d events=%u", fd, events);
}

void IoUringPoller::Remove(int fd) {
    struct io_uring_sqe* sqe = io_uring_get_sqe(&ring_);
    if (!sqe) {
        LOG(Error, "io_uring_get_sqe failed for Remove fd=%d", fd);
        return;
    }

    // Use POLL_REMOVE based on user_data logic or fd match
    // Simplified for this architecture: We are cancelling pending polls for this FD
    // liburing < 2.2 uses void*, >= 2.2 uses __u64
    // If version macros are missing, assume old version (void*)
#if !defined(IO_URING_VERSION_MAJOR) || (IO_URING_VERSION_MAJOR < 2) || (IO_URING_VERSION_MAJOR == 2 && IO_URING_VERSION_MINOR < 2)
    io_uring_prep_poll_remove(sqe, (void*)(intptr_t)fd);
#else
    io_uring_prep_poll_remove(sqe, (__u64)fd);
#endif
    io_uring_submit(&ring_);
    LOG(Trace, "io_uring remove fd=%d", fd);
}

void IoUringPoller::SubmitPoll(int fd, uint32_t events, void* data, int /*op*/) {
    struct io_uring_sqe* sqe = io_uring_get_sqe(&ring_);
    if (!sqe) {
        // Queue full, simplistic handling
        io_uring_submit(&ring_);
        sqe = io_uring_get_sqe(&ring_);
        if (!sqe) return;
    }

    uint32_t poll_mask = 0;
    // Map epoll-like events to io_uring poll mask
    // Note: Constants are compatible with poll.h usually
    if (events & 0x001) poll_mask |= POLLIN; // EPOLLIN
    if (events & 0x004) poll_mask |= POLLOUT; // EPOLLOUT

    io_uring_prep_poll_add(sqe, fd, poll_mask);
    io_uring_sqe_set_data(sqe, data);
}

int IoUringPoller::Poll(std::vector<Event>& active_events, int timeout_ms) {
    struct __kernel_timespec ts;
    ts.tv_sec = timeout_ms / 1000;
    ts.tv_nsec = (timeout_ms % 1000) * 1000000;

    struct io_uring_cqe* cqe;
    // Submit any pending SQEs
    io_uring_submit(&ring_);

    // Wait for at least 1 event
    int ret = io_uring_wait_cqe_timeout(&ring_, &cqe, &ts);
    
    if (ret < 0) {
        if (ret == -ETIME) return 0;
        return -1;
    }

    // Process completions
    unsigned head;
    int count = 0;
    
    io_uring_for_each_cqe(&ring_, head, cqe) {
        if (cqe->res >= 0) {
            Event ev;
            ev.data = io_uring_cqe_get_data(cqe);
            ev.events = 0;
            // Map result back
            if (cqe->res & POLLIN) ev.events |= 0x001; // EPOLLIN
            if (cqe->res & POLLOUT) ev.events |= 0x004; // EPOLLOUT
            
            active_events.push_back(ev);
            count++;
        }
        // In this simple model we re-arm in the event loop logic if needed
        // or if we used multi-shot we wouldn't need to.
    }
    
    io_uring_cq_advance(&ring_, count);
    return count;
}

} // namespace event
} // namespace core
} // namespace networklib
