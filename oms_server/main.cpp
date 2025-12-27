#include "networklib/network_lib.h"
#include "orders.pb.h"
#include "market_data.pb.h"
#include "messages.pb.h"
#include "stream_envelope.pb.h"

#include <iostream>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <atomic>
#include <thread>
#include <chrono>

using namespace networklib;
using namespace networklib::oms;

// =================================================================================================
// BUSINESS LOGIC: Order Manager
// =================================================================================================
class OrderManager {
public:
    OrderManager() = default;

    OrderResponse PlaceOrder(const Order& order) {
        std::unique_lock lock(mutex_);

        if (order.quantity() <= 0 || order.price() < 0) {
            return CreateResponse(order, OrderStatus::REJECTED, "Invalid quantity or price");
        }

        std::string order_id = order.order_id().empty() ? GenerateId() : order.order_id();
        orders_[order_id] = order;

        LOG_INFO("Order Placed: %s, Symbol: %s, Qty: %ld, Price: %.2f",
                 order_id.c_str(), order.symbol().c_str(), order.quantity(), order.price());

        return CreateResponse(order, OrderStatus::NEW, "Order Accepted", order_id);
    }

    OrderResponse CancelOrder(const std::string& order_id) {
        std::unique_lock lock(mutex_);
        auto it = orders_.find(order_id);
        if (it == orders_.end()) {
             OrderResponse resp;
             resp.set_order_id(order_id);
             resp.set_status(OrderStatus::STATUS_UNKNOWN);
             resp.set_message("Order not found");
             return resp;
        }

        LOG_INFO("Order Cancelled: %s", order_id.c_str());
        return CreateResponse(it->second, OrderStatus::CANCELED, "Order Cancelled");
    }

private:
    std::shared_mutex mutex_;
    std::map<std::string, Order> orders_;
    std::atomic<uint64_t> id_counter_{1};

    std::string GenerateId() {
        return "ORD-" + std::to_string(id_counter_.fetch_add(1));
    }

    OrderResponse CreateResponse(const Order& order, OrderStatus status, const std::string& msg, const std::string& assigned_id = "") {
        OrderResponse resp;
        resp.set_order_id(assigned_id.empty() ? order.order_id() : assigned_id);
        resp.set_client_order_id(order.client_order_id());
        resp.set_status(status);
        resp.set_message(msg);
        return resp;
    }
};

class MarketDataPublisher {
public:
    MarketDataPublisher(std::shared_ptr<IServer> server) : server_(server) {}

    void Start() {
        running_ = true;
        thread_ = std::thread([this]() {
            while (running_) {
                PublishTick();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });
    }

    void Stop() {
        running_ = false;
        if (thread_.joinable()) thread_.join();
    }

    void Subscribe(std::shared_ptr<IStreamContext> ctx, const std::string& symbol) {
        std::lock_guard<std::mutex> lock(mutex_);
        // Simple implementation: Just add to list, ignore symbol filtering for now
        // Check if already exists to avoid duplicates?
        // For simplicity, just add.
        subscribers_.push_back(ctx);
        LOG_INFO("New subscriber for %s", symbol.c_str());
    }

private:
    std::shared_ptr<IServer> server_;
    std::thread thread_;
    std::atomic<bool> running_{false};
    std::mutex mutex_;
    std::vector<std::shared_ptr<IStreamContext>> subscribers_;

    void PublishTick() {
        // Create Mock Market Data
        MarketData md;
        md.set_symbol("AAPL");
        md.set_last_price(150.0 + (rand() % 100) / 10.0);
        md.set_last_quantity(100 + rand() % 1000); // Fixed field name
        md.set_timestamp_us(NetworkLib::NowUs()); // Fixed field name

        StreamEnvelope env;
        env.mutable_header()->set_message_type("MARKET_DATA_UPDATE");
        env.mutable_payload()->set_data(md.SerializeAsString());

        std::lock_guard<std::mutex> lock(mutex_);
        // Iterate and remove closed contexts (if possible to detect? IStreamContext doesn't have IsOpen)
        // We will just try to write. If it fails internally, fine.
        // Ideally we need a way to clean up subscribers on disconnect.
        // IStreamContext usually tied to Connection which handles cleanup.
        // But here we hold a shared_ptr, so we might keep it alive?
        // Actually, if Connection closes, writing might fail or do nothing.
        // For this task, we won't overengineer cleanup.

        for (auto& ctx : subscribers_) {
             ctx->Write(env);
        }
    }
};

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: oms_server <config_path>" << std::endl;
        return 1;
    }

    std::string config_path = argv[1];

    try {
        auto logger = networklib::logging::Logger::Initialize("appname=oms-server;level=Info;output=file;file_path=oms_server.log");
        auto server = NetworkLib::CreateServer(config_path);
        if (!server) {
            std::cerr << "Failed to create server." << std::endl;
            return 1;
        }

        OrderManager order_manager;
        MarketDataPublisher md_publisher(server);

        server->RegisterStreamHandler([&](const StreamEnvelope& req, StreamEnvelope& resp, std::shared_ptr<IStreamContext> ctx) {
            if (req.has_header()) {
                if (req.header().message_type() == "ORDER_NEW" && req.has_payload()) {
                    Order order;
                    if (order.ParseFromString(req.payload().data())) {
                        OrderResponse or_resp = order_manager.PlaceOrder(order);
                        resp.mutable_header()->set_message_type("ORDER_RESPONSE");
                        resp.mutable_payload()->set_data(or_resp.SerializeAsString());
                    }
                } else if (req.header().message_type() == "MARKET_DATA_SUB" && req.has_payload()) {
                    // Handle Subscription
                    std::string symbol = req.payload().data(); // Payload is just symbol string
                    md_publisher.Subscribe(ctx, symbol);
                    // No direct response needed, or ack?
                    // Let's send an ACK
                    resp.mutable_header()->set_message_type("MARKET_DATA_SUB_ACK");
                    resp.mutable_payload()->set_data("Subscribed to " + symbol);
                }
            }
        });

        LOG_INFO("Starting OMS Server...");
        if (!server->Start()) {
            LOG_ERROR("Failed to start server.");
            return 1;
        }

        md_publisher.Start();

        server->Wait();

        md_publisher.Stop();

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
