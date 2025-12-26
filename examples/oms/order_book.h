#ifndef NETWORKLIB_EXAMPLES_OMS_ORDER_BOOK_H
#define NETWORKLIB_EXAMPLES_OMS_ORDER_BOOK_H

#include <map>
#include <mutex>
#include <vector>
#include <string>
#include <iostream>

namespace oms {

struct Order {
    std::string id;
    std::string symbol;
    double price;
    int quantity;
    bool is_buy;
    std::string status; // NEW, FILLED, PARTIAL
};

struct Execution {
    std::string symbol;
    double price;
    int quantity;
    std::string buy_order_id;
    std::string sell_order_id;
    int64_t timestamp;
};

class OrderBook {
public:
    static OrderBook& Instance() {
        static OrderBook instance;
        return instance;
    }

    void AddOrder(Order order) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Simple Matching Logic
        bool matched = false;
        (void)matched;
        // Iterate opposite side to match
        // Note: Very naive O(N) matching for Phase 21 demo. Real OMS uses O(1) price levels.
        
        for (auto& resting : orders_) {
            if (resting.status == "FILLED") continue;
            if (resting.symbol != order.symbol) continue;
            if (resting.is_buy == order.is_buy) continue; // Same side
            
            // Check Price
            bool price_match = order.is_buy ? (order.price >= resting.price) : (order.price <= resting.price);
            
            if (price_match) {
                // Execute
                int exec_qty = std::min(order.quantity, resting.quantity);
                
                Execution exec;
                exec.symbol = order.symbol;
                exec.price = resting.price; // Maker price
                exec.quantity = exec_qty;
                exec.buy_order_id = order.is_buy ? order.id : resting.id;
                exec.sell_order_id = order.is_buy ? resting.id : order.id;
                
                // Update quantities
                order.quantity -= exec_qty;
                resting.quantity -= exec_qty;
                
                if (resting.quantity == 0) resting.status = "FILLED";
                
                NotifyExecution(exec);
                
                if (order.quantity == 0) {
                    order.status = "FILLED";
                    matched = true;
                    break;
                }
            }
        }
        
        if (order.quantity > 0) {
            order.status = "NEW";
            orders_.push_back(order);
        }
        
        NotifyListeners(order);
    }

    void UpdatePrice(const std::string& symbol, double price) {
        std::lock_guard<std::mutex> lock(mutex_);
        prices_[symbol] = price;
        
        Order update;
        update.symbol = symbol;
        update.price = price;
        update.quantity = 0; 
        update.id = "quote";
        update.status = "QUOTE";
        NotifyListeners(update);
    }

    void Subscribe(std::function<void(const Order&)> callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        listeners_.push_back(std::move(callback));
    }

    void SubscribeExecutions(std::function<void(const Execution&)> callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        exec_listeners_.push_back(std::move(callback));
    }

private:
    void NotifyListeners(const Order& order) {
        for (const auto& cb : listeners_) {
            cb(order);
        }
    }

    void NotifyExecution(const Execution& exec) {
        for (const auto& cb : exec_listeners_) {
            cb(exec);
        }
    }

    std::vector<Order> orders_;
    std::map<std::string, double> prices_;
    std::vector<std::function<void(const Order&)>> listeners_;
    std::vector<std::function<void(const Execution&)>> exec_listeners_;
    std::mutex mutex_;
};

} // namespace oms

#endif // NETWORKLIB_EXAMPLES_OMS_ORDER_BOOK_H
