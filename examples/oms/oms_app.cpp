#include "networklib/network_lib.h"
#include "networklib/config/config.h" // For manual config
#include "stream_envelope.pb.h"
#include <iostream>
#include <vector>
#include "networklib/core/event/reactor.h"
#include "networklib/core/event/event_loop.h"
#include "networklib/protocols/grpc/service_registry.h"
#include "networklib/protocols/grpc/grpc_handler.h"
#include "networklib/logger.h"
#include "market_data.h"
#include "order_service.h"
#include "market_feed.h"

using namespace networklib;

// Market Feed Logic
class MarketFeed {
public:
    void HandleSubscription(const StreamEnvelope& /*req*/, StreamEnvelope& resp, std::shared_ptr<IStreamContext> ctx) {
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
    logging::Logger::Initialize("appname=OMS;console=true;severity=Info");
    logging::Logger::Log(logging::LogLevel::Info, __FILE__, __LINE__, __FUNCTION__, "Starting OMS Reference Application...");

    auto loop = std::make_unique<core::event::EventLoop>();
    loop->Init();
    core::event::Reactor reactor(std::move(loop));

    // 1. Market Data (UDP 9000)
    auto md_handler = std::make_shared<oms::MarketDataHandler>();
    reactor.RegisterUdpServer(9000, md_handler);
    logging::Logger::Log(logging::LogLevel::Info, __FILE__, __LINE__, __FUNCTION__, " - Market Data (UDP) on 9000");

    // 2. Order Entry (gRPC 50051)
    oms::OrderEntryService orderService;
    protocols::grpc::ServiceRegistry::Instance().RegisterMethod(
        "/oms.OrderEntry/StreamOrders", 
        [&](std::shared_ptr<protocols::grpc::GrpcStream> s) { orderService.HandleStream(s); }
    );
    auto grpc_handler = std::make_shared<protocols::grpc::GrpcHandler>();
    reactor.RegisterServer(50051, grpc_handler);
    logging::Logger::Log(logging::LogLevel::Info, __FILE__, __LINE__, __FUNCTION__, " - Order Entry (gRPC) on 50051");

    // 3. Market Feed (WebSocket 8080)
    auto ws_handler = std::make_shared<oms::MarketFeedHandler>();
    reactor.RegisterServer(8080, ws_handler);
    logging::Logger::Log(logging::LogLevel::Info, __FILE__, __LINE__, __FUNCTION__, " - Market Feed (WebSocket) on 8080");

    logging::Logger::Log(logging::LogLevel::Info, __FILE__, __LINE__, __FUNCTION__, "OMS Running. Press Ctrl+C to stop.");
    reactor.Run();

    logging::Logger::Deinitialize();

    return 0;
}
