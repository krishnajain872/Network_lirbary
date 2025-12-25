#include <iostream>
#include <unistd.h>
#include <thread>
#include <cassert>
#include "networklib/core/event/event_loop.h"
#include <sys/eventfd.h>

using namespace networklib::core::event;

void TestEventLoop() {
    EventLoop loop;
    auto res = loop.Init();
    assert(res);

    int efd = eventfd(0, EFD_NONBLOCK);
    bool callback_fired = false;

    loop.AddFd(efd, EPOLLIN, [&](uint32_t events) {
        uint64_t u;
        read(efd, &u, sizeof(uint64_t));
        callback_fired = true;
        loop.Stop();
    });

    // Write to eventfd in a separate thread to trigger the event
    std::thread t([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        uint64_t u = 1;
        write(efd, &u, sizeof(uint64_t));
    });

    loop.Run();
    t.join();
    close(efd);

    assert(callback_fired);
    std::cout << "TestEventLoop PASSED" << std::endl;
}

int main() {
    TestEventLoop();
    return 0;
}
