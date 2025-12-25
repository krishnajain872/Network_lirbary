#ifndef NETWORKLIB_EXAMPLES_OMS_ORDER_SERVICE_H
#define NETWORKLIB_EXAMPLES_OMS_ORDER_SERVICE_H

#include "networklib/protocols/grpc/grpc_stream.h"
#include "networklib/core/resilience/token_bucket.h"
#include "order_book.h"
#include <sstream>

namespace oms {

using namespace networklib::protocols::grpc;
using namespace networklib::core::resilience;

class OrderEntryService {
public:
    // Rate limiter: 1000 orders/sec
    OrderEntryService() : limiter_(std::make_shared<TokenBucket>(1000.0, 100.0)) {}

    void HandleStream(std::shared_ptr<GrpcStream> stream) {
        // Capture limiter by shared_ptr to keep alive
        auto limiter = limiter_;
        
        stream->SetOnMessage([stream, limiter](const std::string& msg) {
            // Rate Limit Check
            if (!limiter->Consume(1.0)) {
                stream->Write("REJECT RateLimitExceeded");
                return;
            }

            // Parse "BUY AAPL 10 150.00"
            std::stringstream ss(msg);
            std::string side, symbol;
            int qty;
            double price;
            
            if (ss >> side >> symbol >> qty >> price) {
                Order order;
                order.id = "ord-" + std::to_string(std::rand());
                order.symbol = symbol;
                order.quantity = qty;
                order.price = price;
                order.is_buy = (side == "BUY");
                
                OrderBook::Instance().AddOrder(order);
                stream->Write("ACK " + order.id);
            } else {
                stream->Write("REJECT InvalidFormat");
            }
        });
    }

private:
    std::shared_ptr<TokenBucket> limiter_;
};

} // namespace oms

#endif // NETWORKLIB_EXAMPLES_OMS_ORDER_SERVICE_H
