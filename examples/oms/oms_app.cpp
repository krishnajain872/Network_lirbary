#include "networklib/network_lib.h"
#include "networklib/config/config.h" // For manual config
#include "stream_envelope.pb.h"
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <set>

using namespace networklib;

// Market Feed Logic
class MarketFeed {
public:
    void HandleSubscription(const StreamEnvelope& req, StreamEnvelope& resp, std::shared_ptr<IStreamContext> ctx) {
        std::cout << "[Feed] New subscriber" << std::endl;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            subscribers_.push_back(ctx);
        }
        resp.mutable_payload()->set_data("Subscribed to Market Feed");
    }

    void Broadcast(const std::string& msg) {
        std::lock_guard<std::mutex> lock(mutex_);
        StreamEnvelope env;
        env.mutable_payload()->set_data(msg);
        for (auto& ctx : subscribers_) {
            ctx->Write(env);
        }
    }

private:
    std::mutex mutex_;
    std::vector<std::shared_ptr<IStreamContext>> subscribers_;
};

int main() {
    std::cout << "Starting OMS..." << std::endl;
    std::vector<std::shared_ptr<IServer>> servers;

    // 1. Order Entry Server (Port 50051)
    config::ServerConfig order_cfg;
    order_cfg.network.port = 50051;
    order_cfg.mode = "tcp"; // Generic TCP/gRPC

    auto order_server = NetworkLib::CreateServer(order_cfg);
    order_server->RegisterStreamHandler([](const StreamEnvelope& req, StreamEnvelope& resp, std::shared_ptr<IStreamContext> ctx) {
        std::cout << "[Order] Recv: " << req.payload().data() << std::endl;
        resp.mutable_payload()->set_data("ACK-ORDER-123");
    });
    servers.push_back(order_server);

    // 2. Market Feed Server (Port 8080 - WS)
    config::ServerConfig feed_cfg;
    feed_cfg.network.port = 8080;
    feed_cfg.mode = "websocket";

    MarketFeed feed;
    auto feed_server = NetworkLib::CreateServer(feed_cfg);
    feed_server->RegisterStreamHandler([&feed](const StreamEnvelope& req, StreamEnvelope& resp, std::shared_ptr<IStreamContext> ctx) {
        feed.HandleSubscription(req, resp, ctx);
    });
    servers.push_back(feed_server);

    // 3. Market Data Generator
    std::thread market_thread([&feed]() {
        int price = 100;
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            price += (std::rand() % 5) - 2;
            std::string update = "AAPL " + std::to_string(price);
            feed.Broadcast(update);
        }
    });

    // Start all
    for(auto& s : servers) s->Start();

    std::cout << "OMS Running. Press Enter to exit." << std::endl;
    std::cin.get();

    for(auto& s : servers) s->Stop();
    return 0;
}
