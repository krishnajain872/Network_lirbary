#ifndef NETWORKLIB_CORE_CONFIG_HOT_RELOAD_H
#define NETWORKLIB_CORE_CONFIG_HOT_RELOAD_H

#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include "networklib/config/config.h"

namespace networklib {
namespace config {

class ConfigHotReload {
public:
    using ReloadCallback = std::function<void(const ServerConfig&)>;

    ConfigHotReload(const std::string& filepath, ReloadCallback callback);
    ~ConfigHotReload();

    void Start();
    void Stop();

private:
    void WatchLoop();

    std::string filepath_;
    ReloadCallback callback_;
    std::thread watch_thread_;
    std::atomic<bool> running_;
    int inotify_fd_;
    int watch_fd_;
};

} // namespace config
} // namespace networklib

#endif // NETWORKLIB_CORE_CONFIG_HOT_RELOAD_H
