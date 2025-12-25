#ifndef NETWORKLIB_CORE_EVENT_EVENT_LOOP_H
#define NETWORKLIB_CORE_EVENT_EVENT_LOOP_H

#include <functional>
#include <map>
#include <vector>
#include <memory>
#include <string>

#include "networklib/utils/result.h"
#include "networklib/core/event/poller.h"

namespace networklib {
namespace core {
namespace event {

/**
 * @brief Structure representing a monitored file descriptor event.
 * Re-declared here for legacy compatibility inside EventLoop context,
 * though Poller uses its own internal Event struct.
 */
struct EventHandler {
    int fd;
    uint32_t events;
    std::function<void(uint32_t)> callback;
};

/**
 * @brief High-performance event loop using configurable Poller (epoll/io_uring).
 * 
 * Manages asynchronous I/O events for the network library.
 */
class EventLoop {
public:
    enum class PollerType {
        EPOLL,
        IO_URING
    };

    EventLoop();
    ~EventLoop();

    /**
     * @brief Initializes the event loop with a specific poller backend.
     * @param type The poller type to use (default: EPOLL).
     * @return Result<void> Success or failure.
     */
    utils::Result<void> Init(PollerType type = PollerType::EPOLL);

    /**
     * @brief Starts the event loop.
     * Blocks until Stop() is called.
     */
    void Run();

    /**
     * @brief Stops the event loop.
     */
    void Stop();

    utils::Result<void> AddFd(int fd, uint32_t events, std::function<void(uint32_t)> callback);
    utils::Result<void> ModifyFd(int fd, uint32_t events);
    void RemoveFd(int fd);

private:
    std::unique_ptr<Poller> poller_;
    bool running_;
    std::map<int, std::shared_ptr<EventHandler>> handlers_; 
};

} // namespace event
} // namespace core
} // namespace networklib

#endif // NETWORKLIB_CORE_EVENT_EVENT_LOOP_H
