#include "networklib/core/event/event_loop.h"
#include "networklib/core/event/epoll_poller.h"
#include "networklib/core/event/io_uring_poller.h"
#include <sys/eventfd.h>
#include <unistd.h>
#include <iostream>
#include "networklib/constants/errors.h"
#include "networklib/logger.h"
#include <iostream>
#include <cstring>
#include <sys/epoll.h>

namespace networklib {
namespace core {
namespace event {

EventLoop::EventLoop() : running_(false), wakeup_fd_(-1), thread_id_(std::this_thread::get_id()) {}

EventLoop::~EventLoop() {
    Stop();
    if (wakeup_fd_ >= 0) {
        close(wakeup_fd_);
    }
}

utils::Result<void> EventLoop::Init(PollerType type) {
    try {
        if (type == PollerType::IO_URING) {
            poller_ = std::make_unique<IoUringPoller>();
        } else {
            poller_ = std::make_unique<EpollPoller>();
        }
    } catch (const std::exception& e) {
        return utils::Result<void>::Failure(
            constants::errors::kInternal,
            std::string("Failed to initialize poller: ") + e.what()
        );
    }

    // Create Wakeup FD
    wakeup_fd_ = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (wakeup_fd_ < 0) {
        LOG(Error, "Failed to create eventfd: %s", strerror(errno));
        return utils::Result<void>::Failure(constants::errors::kInternal, "Failed to create eventfd");
    }

    // Register Wakeup FD
    AddFd(wakeup_fd_, EPOLLIN, [this](uint32_t) {
        HandleRead();
    });

    LOG(Debug, "EventLoop initialized with %s poller", (type == PollerType::IO_URING ? "IO_URING" : "EPOLL"));
    return utils::Result<void>::Success();
}

utils::Result<void> EventLoop::AddFd(int fd, uint32_t events, std::function<void(uint32_t)> callback) {
    if (!poller_) {
        LOG(Error, "EventLoop not initialized. Call Init() first.");
        return utils::Result<void>::Failure(constants::errors::kNotInitialized, "EventLoop not initialized. Call Init() first.");
    }
    auto handler = std::make_shared<EventHandler>(EventHandler{fd, events, callback});
    
    try {
        poller_->Add(fd, events, handler.get());
    } catch (const std::exception& e) {
        LOG(Error, "Failed to add fd %d: %s", fd, e.what());
        return utils::Result<void>::Failure(constants::errors::kInternal, e.what());
    }
    
    handlers_[fd] = handler;
    LOG(Trace, "Added fd %d to loop", fd);
    return utils::Result<void>::Success();
}

utils::Result<void> EventLoop::ModifyFd(int fd, uint32_t events) {
    if (!poller_) {
        return utils::Result<void>::Failure(constants::errors::kNotInitialized, "EventLoop not initialized. Call Init() first.");
    }
    if (handlers_.find(fd) == handlers_.end()) {
        return utils::Result<void>::Failure(constants::errors::kNotFound, "FD not found in loop");
    }
    auto handler = handlers_[fd];
    handler->events = events;

    try {
        poller_->Modify(fd, events, handler.get());
    } catch (const std::exception& e) {
        LOG(Error, "Failed to modify fd %d: %s", fd, e.what());
        return utils::Result<void>::Failure(constants::errors::kInternal, e.what());
    }
    LOG(Trace, "Modified fd %d events", fd);
    return utils::Result<void>::Success();
}

void EventLoop::RemoveFd(int fd) {
    if (!poller_) return;
    if (handlers_.find(fd) != handlers_.end()) {
        poller_->Remove(fd);
        handlers_.erase(fd);
        LOG(Trace, "Removed fd %d from loop", fd);
    }
}

void EventLoop::Run() {
    if (!poller_) {
        LOG(Error, "EventLoop::Run() called without Init()");
        return;
    }
    LOG(Info, "Event loop starting");
    running_ = true;
    std::vector<Event> events;
    events.reserve(128);

    while (running_) {
        events.clear();
        int count = poller_->Poll(events, -1);
        
        if (count < 0) {
            if (errno != EINTR) {
                LOG(Error, "Poller returned error: %s", strerror(errno));
            }
            break;
        }

        for (const auto& ev : events) {
            EventHandler* raw_handler = static_cast<EventHandler*>(ev.data);

            // Fix: Extend lifetime of handler during callback execution
            // We look up the shared_ptr in the map to ensure it's still valid and to hold a strong ref
            // If the handler removes itself (e.g., HandleClose -> RemoveFd), this strong ref keeps it alive until return.

            if (!raw_handler) continue;

            int fd = raw_handler->fd;
            auto it = handlers_.find(fd);
            if (it == handlers_.end()) {
                // Handler was removed by a previous event in this loop iteration?
                continue;
            }

            std::shared_ptr<EventHandler> handler = it->second;

            if (handler && handler->callback) {
                // Map generic event flags back to epoll-style constants expected by callbacks
                // Currently our Poller impl maps 0x001->EPOLLIN etc implicitly in the callbacks usage
                // TODO: Define abstraction for EventFlags
                uint32_t flags = 0;
                if (ev.events & 0x001) flags |= EPOLLIN;
                if (ev.events & 0x004) flags |= EPOLLOUT;
                
                handler->callback(flags);
            }
        }
    }
    LOG(Info, "Event loop stopped");
}

void EventLoop::Stop() {
    LOG(Info, "Stopping event loop");
    running_ = false;
    WakeUp();
}

void EventLoop::RunInLoop(std::function<void()> task) {
    if (IsInLoopThread()) {
        task();
    } else {
        QueueInLoop(std::move(task));
    }
}

void EventLoop::QueueInLoop(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pending_tasks_.push_back(std::move(task));
    }
    WakeUp();
}

void EventLoop::WakeUp() {
    uint64_t one = 1;
    ssize_t n = write(wakeup_fd_, &one, sizeof(one));
    (void)n;
}

void EventLoop::HandleRead() {
    uint64_t one;
    ssize_t n = read(wakeup_fd_, &one, sizeof(one));
    (void)n;

    std::vector<std::function<void()>> tasks;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        tasks.swap(pending_tasks_);
    }

    for (const auto& task : tasks) {
        task();
    }
}

bool EventLoop::IsInLoopThread() const {
    return thread_id_ == std::this_thread::get_id();
}

} // namespace event
} // namespace core
} // namespace networklib
