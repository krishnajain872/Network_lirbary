#ifndef NETWORKLIB_EXAMPLES_OMS_MARKET_FEED_H
#define NETWORKLIB_EXAMPLES_OMS_MARKET_FEED_H

#include "networklib/protocols/websocket/websocket_handler.h"
#include "order_book.h"
#include <set>

namespace oms {

using namespace networklib::protocols::websocket;
using namespace networklib::core;

class MarketFeedHandler : public WebSocketHandler {
public:
    MarketFeedHandler() {
        // Subscribe to OrderBook updates
        // Note: In real app, we need to manage subscription lifecycle (remove on disconnect)
        // This is a simplified leak for Phase 18 demo
        // We need a shared state or static manager to map Connection -> Callback
    }

    void OnConnection(const Connection::Ptr& conn) override {
        WebSocketHandler::OnConnection(conn);
        
        // Subscribe this connection to updates
        auto weak_conn = std::weak_ptr<Connection>(conn);
        OrderBook::Instance().Subscribe([weak_conn](const Order& order) {
            if (auto conn = weak_conn.lock()) {
                if (conn->IsConnected()) {
                    std::string msg = "FEED: " + order.symbol + " " + std::to_string(order.price);
                    
                    // Manual WS Frame construction (Text)
                    std::vector<char> frame;
                    frame.push_back(0x81);
                    frame.push_back(static_cast<uint8_t>(msg.size()));
                    frame.insert(frame.end(), msg.begin(), msg.end());
                    
                    conn->Send(std::string(frame.begin(), frame.end()));
                }
            }
        });
    }
};

} // namespace oms

#endif // NETWORKLIB_EXAMPLES_OMS_MARKET_FEED_H
