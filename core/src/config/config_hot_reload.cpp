#include "networklib/core/config/config_hot_reload.h"
#include "networklib/logging.h"
#include <sys/inotify.h>
#include <unistd.h>
#include <iostream>
#include <limits.h>

namespace networklib {
namespace config {

ConfigHotReload::ConfigHotReload(const std::string& filepath, ReloadCallback callback)
    : filepath_(filepath), callback_(callback), running_(false), inotify_fd_(-1), watch_fd_(-1) {
}

ConfigHotReload::~ConfigHotReload() {
    Stop();
}

void ConfigHotReload::Start() {
    if (running_) return;
    running_ = true;
    
    inotify_fd_ = inotify_init1(IN_NONBLOCK);
    if (inotify_fd_ < 0) {
        logging::Logger::Log(logging::LogLevel::Error, __FILE__, __LINE__, __FUNCTION__, "Failed to init inotify");
        return;
    }

    // Watch for Modify
    watch_fd_ = inotify_add_watch(inotify_fd_, filepath_.c_str(), IN_MODIFY | IN_CLOSE_WRITE);
    
    watch_thread_ = std::thread(&ConfigHotReload::WatchLoop, this);
}

void ConfigHotReload::Stop() {
    running_ = false;
    if (watch_thread_.joinable()) {
        watch_thread_.join();
    }
    if (watch_fd_ >= 0) inotify_rm_watch(inotify_fd_, watch_fd_);
    if (inotify_fd_ >= 0) close(inotify_fd_);
}

void ConfigHotReload::WatchLoop() {
    char buffer[4096] __attribute__((aligned(__alignof__(struct inotify_event))));
    const struct inotify_event *event;
    ssize_t len;

    while (running_) {
        len = read(inotify_fd_, buffer, sizeof(buffer));
        if (len <= 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            continue;
        }

        char *ptr;
        for (ptr = buffer; ptr < buffer + len; ptr += sizeof(struct inotify_event) + event->len) {
            event = (const struct inotify_event *)ptr;
            if (event->mask & (IN_MODIFY | IN_CLOSE_WRITE)) {
                logging::Logger::Log(logging::LogLevel::Info, __FILE__, __LINE__, __FUNCTION__, "[Config] Change detected, reloading...");
                try {
                    ServerConfig new_config = ConfigParser::Parse(filepath_);
                    if (callback_) {
                        callback_(new_config);
                    }
                } catch (const std::exception& e) {
                    logging::Logger::Log(logging::LogLevel::Error, __FILE__, __LINE__, __FUNCTION__, "[Config] Reload failed: %s", e.what());
                }
            }
        }
    }
}

} // namespace config
} // namespace networklib
