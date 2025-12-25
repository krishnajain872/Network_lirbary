#include <iostream>
#include <thread>
#include <vector>
#include "networklib/core/event/reactor.h"
#include "networklib/core/event/event_loop.h"
#include "networklib/protocols/grpc/service_registry.h"
#include "networklib/protocols/grpc/grpc_handler.h"
#include "market_data.h"
#include "order_service.h"
#include "market_feed.h"

using namespace networklib;

int main() {
    std::cout << "Starting OMS Reference Application..." << std::endl;

    auto loop = std::make_unique<core::event::EventLoop>();
    core::event::Reactor reactor(std::move(loop));

    // 1. Market Data (UDP 9000)
    auto md_handler = std::make_shared<oms::MarketDataHandler>();
    reactor.RegisterUdpServer(9000, md_handler);
    std::cout << " - Market Data (UDP) on 9000" << std::endl;

    // 2. Order Entry (gRPC 50051)
    oms::OrderEntryService orderService;
    protocols::grpc::ServiceRegistry::Instance().RegisterMethod(
        "/oms.OrderEntry/StreamOrders", 
        [&](std::shared_ptr<protocols::grpc::GrpcStream> s) { orderService.HandleStream(s); }
    );
    auto grpc_handler = std::make_shared<protocols::grpc::GrpcHandler>();
    reactor.RegisterServer(50051, grpc_handler);
    std::cout << " - Order Entry (gRPC) on 50051" << std::endl;

    // 3. Market Feed (WebSocket 8080)
    auto ws_handler = std::make_shared<oms::MarketFeedHandler>();
    reactor.RegisterServer(8080, ws_handler);
    std::cout << " - Market Feed (WebSocket) on 8080" << std::endl;

    std::cout << "OMS Running. Press Ctrl+C to stop." << std::endl;
    reactor.Run();

    return 0;
}
