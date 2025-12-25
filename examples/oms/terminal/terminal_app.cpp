#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <sstream>
#include <chrono>
#include <atomic>
#include "networklib/protocols/grpc/grpc_client.h"
#include "networklib/core/event/event_loop.h"
#include "oms_definitions.pb.h"
#include "stream_envelope.pb.h"

// Terminal Client
// Commands:
// - BUY <SYMBOL> <QTY> <PRICE>
// - BULK_BUY <COUNT>
// - LOAD_TEST <CLIENTS> <ORDERS_PER_CLIENT>

using namespace networklib;
using namespace networklib::oms;

// Helper to serialize specific proto to Generic Envelope payload
std::string SerializeToEnvelope(const google::protobuf::Message& msg, const std::string& type) {
    StreamEnvelope envelope;
    envelope.mutable_header()->set_message_type(type);
    envelope.mutable_header()->set_message_id("msg-" + std::to_string(std::rand()));
    envelope.mutable_header()->set_timestamp_us(std::chrono::system_clock::now().time_since_epoch().count());
    
    envelope.mutable_payload()->set_content_type("application/protobuf");
    envelope.mutable_payload()->set_data(msg.SerializeAsString());
    
    return envelope.SerializeAsString();
}

void RunLoadTest(int client_count, int orders_per_client) {
    std::cout << "[LoadTest] Starting " << client_count << " clients, " << orders_per_client << " orders each..." << std::endl;
    
    std::vector<std::thread> threads;
    std::atomic<int> completed_clients{0};
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < client_count; ++i) {
        threads.emplace_back([i, orders_per_client, &completed_clients]() {
            auto loop = std::make_shared<core::event::EventLoop>();
            protocols::grpc::GrpcClient client(loop);
            if (!client.Connect("127.0.0.1", 50051)) {
                std::cerr << "Client " << i << " failed to connect." << std::endl;
                return;
            }
            
            // Send orders
            for (int j = 0; j < orders_per_client; ++j) {
                OrderBuy buy;
                buy.set_symbol("BTC-USD");
                buy.set_price(100.0 + (j % 10));
                buy.set_quantity(1.0);
                buy.set_type(OrderType::LIMIT);
                buy.set_client_order_id("client-" + std::to_string(i) + "-ord-" + std::to_string(j));
                
                std::string payload = SerializeToEnvelope(buy, "oms.OrderBuy");
                client.SendUnary("/oms.OrderEntry/PlaceOrder", payload);
                
                // Small sleep to not overwhelm local loop immediately
                // std::this_thread::sleep_for(std::chrono::microseconds(10)); 
            }
            completed_clients++;
        });
    }
    
    for (auto& t : threads) t.join();
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    
    std::cout << "[LoadTest] Completed in " << duration_ms << "ms" << std::endl;
    double total_reqs = (double)client_count * orders_per_client;
    double rps = total_reqs / (duration_ms / 1000.0);
    std::cout << "[LoadTest] Throughput: " << rps << " req/s" << std::endl;
}

int main(int argc, char** argv) {
    auto loop = std::make_shared<core::event::EventLoop>();
    protocols::grpc::GrpcClient client(loop);
    
    std::string host = "127.0.0.1";
    int port = 50051;
    
    std::cout << "Connecting to OMS Order Entry at " << host << ":" << port << "..." << std::endl;
    if (!client.Connect(host, port)) {
        std::cerr << "Failed to connect to OMS." << std::endl;
        return 1;
    }
    
    std::cout << "--- OMS Terminal ---" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  BUY <SYMBOL> <QTY> <PRICE>" << std::endl;
    std::cout << "  SELL <SYMBOL> <QTY> <PRICE>" << std::endl;
    std::cout << "  BULK_BUY <COUNT> (Single Batch Request)" << std::endl;
    std::cout << "  LOAD_TEST <CLIENTS> <ORDERS_PER_CLIENT> (Concurrent Connections)" << std::endl;
    std::cout << "  EXIT" << std::endl;
    
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line == "EXIT") break;
        if (line.empty()) continue;
        
        std::stringstream ss(line);
        std::string cmd;
        ss >> cmd;
        
        if (cmd == "BUY" || cmd == "SELL") {
            std::string sym;
            double qty, px;
            ss >> sym >> qty >> px;
            
            if (cmd == "BUY") {
                OrderBuy msg;
                msg.set_symbol(sym);
                msg.set_quantity(qty);
                msg.set_price(px);
                msg.set_type(OrderType::LIMIT);
                client.SendUnary("/oms.OrderEntry/PlaceOrder", SerializeToEnvelope(msg, "oms.OrderBuy"));
            } else {
                OrderSell msg;
                msg.set_symbol(sym);
                msg.set_quantity(qty);
                msg.set_price(px);
                msg.set_type(OrderType::LIMIT);
                client.SendUnary("/oms.OrderEntry/PlaceOrder", SerializeToEnvelope(msg, "oms.OrderSell"));
            }
        }
        else if (cmd == "BULK_BUY") {
            int count;
            ss >> count;
            BulkOrderRequest bulk;
            bulk.set_atomic(false);
            for(int i=0; i<count; ++i) {
                auto* o = bulk.add_buy_orders();
                o->set_symbol("BTC-USD");
                o->set_quantity(1.0);
                o->set_price(100.0);
                o->set_type(OrderType::MARKET);
            }
            std::cout << "Sending Batch of " << count << " orders..." << std::endl;
            client.SendUnary("/oms.OrderEntry/PlaceBulkOrder", SerializeToEnvelope(bulk, "oms.BulkOrderRequest"));
        }
        else if (cmd == "LOAD_TEST") {
            int clients, orders;
            ss >> clients >> orders;
            RunLoadTest(clients, orders);
        }
        else {
            std::cout << "Unknown command." << std::endl;
        }
    }
    
    return 0;
}
