#ifndef NETWORKLIB_EXAMPLES_OMS_MARKET_DATA_H
#define NETWORKLIB_EXAMPLES_OMS_MARKET_DATA_H

#include "networklib/protocols/udp/udp_handler.h"
#include "order_book.h"
#include <sstream>

namespace oms {

using namespace networklib::protocols::udp;

class MarketDataHandler : public UdpHandler {
public:
    void OnPacket(int fd, const char* data, size_t len, const std::string& source_ip, int source_port) override {
        // Format: "SYMBOL PRICE" (e.g., "AAPL 150.50")
        std::string msg(data, len);
        std::stringstream ss(msg);
        std::string symbol;
        double price;
        
        if (ss >> symbol >> price) {
            OrderBook::Instance().UpdatePrice(symbol, price);
        }
    }
};

} // namespace oms

#endif // NETWORKLIB_EXAMPLES_OMS_MARKET_DATA_H
