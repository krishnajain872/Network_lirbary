#include "networklib/network_lib.h"
#include "networklib/config/config.h"
#include "stream_envelope.pb.h"
#include <iostream>
#include <thread>
#include <string>

using namespace networklib;

void MarketDataConsumer() {
    config::ClientConfig cfg;
    cfg.network.port = 8080;
    cfg.mode = "websocket";
    
    auto client = NetworkLib::CreateClient("oms_client_feed.yaml"); // or manual config if I expose it
    // CreateClient only takes string path.
    // I need to create the file.
    {
        FILE* f = fopen("oms_client_feed.yaml", "w");
        fprintf(f, "client:\n  mode: websocket\n  connection:\n    port: 8080\n    host: 127.0.0.1\n");
        fclose(f);
    }
    
    // Register Handler
    client->RegisterMessageHandler([](const StreamEnvelope& msg) {
        std::cout << "[Feed] Received: " << msg.payload().data() << std::endl;
    });

    if (client->Connect()) {
        std::cout << "[Feed] Connected. Subscribing..." << std::endl;
        StreamEnvelope sub;
        sub.mutable_payload()->set_data("SUBSCRIBE");
        client->Send(sub);

        // Keep thread alive
        while(true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

int main() {
    // Order Client
    {
        FILE* f = fopen("oms_client_order.yaml", "w");
        fprintf(f, "client:\n  mode: tcp\n  connection:\n    port: 50051\n    host: 127.0.0.1\n");
        fclose(f);
    }
    auto order_client = NetworkLib::CreateClient("oms_client_order.yaml");
    order_client->Connect();

    std::thread feed_thread(MarketDataConsumer);

    while (true) {
        std::cout << "\n=== OMS Terminal ===\n";
        std::cout << "1. Place Order\n";
        std::cout << "2. Exit\n";
        std::cout << "> ";
        
        int choice;
        std::cin >> choice;
        
        if (choice == 1) {
            StreamEnvelope order;
            order.mutable_payload()->set_data("BUY AAPL 10 150.00");
            order_client->Send(order);
            std::cout << "Order sent.\n";
        } else if (choice == 2) {
            break;
        }
    }
    
    order_client->Disconnect();
    feed_thread.join(); // Ideally signal it to stop
    return 0;
}
