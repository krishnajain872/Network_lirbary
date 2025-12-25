#include "networklib/core/event/event_loop.h"
#include "networklib/core/event/epoll_poller.h"
#include "networklib/core/event/io_uring_poller.h"
#include "networklib/constants/errors.h"
#include <iostream>
#include <cstring>
#include <sys/epoll.h>

namespace networklib {
namespace core {
namespace event {

EventLoop::EventLoop() : running_(false) {}

EventLoop::~EventLoop() {
    Stop();
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
    return utils::Result<void>::Success();
}

utils::Result<void> EventLoop::AddFd(int fd, uint32_t events, std::function<void(uint32_t)> callback) {
    if (!poller_) {
        return utils::Result<void>::Failure(constants::errors::kNotInitialized, "EventLoop not initialized. Call Init() first.");
    }
    auto handler = std::make_shared<EventHandler>(EventHandler{fd, events, callback});
    
    try {
        poller_->Add(fd, events, handler.get());
    } catch (const std::exception& e) {
        return utils::Result<void>::Failure(constants::errors::kInternal, e.what());
    }
    
    handlers_[fd] = handler;
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
        return utils::Result<void>::Failure(constants::errors::kInternal, e.what());
    }
    return utils::Result<void>::Success();
}

void EventLoop::RemoveFd(int fd) {
    if (!poller_) return;
    if (handlers_.find(fd) != handlers_.end()) {
        poller_->Remove(fd);
        handlers_.erase(fd);
    }
}

void EventLoop::Run() {
    if (!poller_) {
        std::cerr << "EventLoop::Run() called without Init()" << std::endl;
        return;
    }
    running_ = true;
    std::vector<Event> events;
    events.reserve(128);

    while (running_) {
        events.clear();
        int count = poller_->Poll(events, -1);
        
        if (count < 0) {
            // Error logged by poller or check errno if needed
            // For now break
            break;
        }

        for (const auto& ev : events) {
            EventHandler* handler = static_cast<EventHandler*>(ev.data);
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
}

void EventLoop::Stop() {
    running_ = false;
}

} // namespace event
} // namespace core
} // namespace networklib
