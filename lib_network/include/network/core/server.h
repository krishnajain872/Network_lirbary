#ifndef NETWORKLIB_CORE_SERVER_H
#define NETWORKLIB_CORE_SERVER_H

#include "logger/logging.h"
#include "network/detail/interfaces.h"
#include "network/config/config.h"
#include "network/core/event/reactor.h"
#include "network/core/config/config_hot_reload.h"
#include <thread>
#include <atomic>

namespace networklib {
namespace core {

class Server : public IServer {
public:
    Server(const config::ServerConfig& config);
    ~Server() override;

    bool Start() override;
    void Stop() override;
    void Wait() override;

    void RegisterStreamHandler(StreamHandler handler) override;
    void RegisterRawHandler(RawHandler handler) override;

private:
    config::ServerConfig config_;
    StreamHandler stream_handler_;
    RawHandler raw_handler_;
    std::unique_ptr<event::Reactor> reactor_;
    std::unique_ptr<config::ConfigHotReload> hot_reload_;
    std::thread runner_thread_;
    std::atomic<bool> running_{false};

    void Init();
    void ReloadConfig(const config::ServerConfig& new_conf);
};

} // namespace core
} // namespace networklib

#endif // NETWORKLIB_CORE_SERVER_H
