#include "network_lib.h"
#include "orders.pb.h"
#include "stream_envelope.pb.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace networklib;
using namespace networklib::oms;

void RunOrderPlacementScenario(std::shared_ptr<IClient> client) {
    LOG_INFO("Starting Order Placement Scenario...");

    for (int i = 0; i < 10; ++i) {
        Order order;
        order.set_order_id("ORD-" + std::to_string(i));
        order.set_symbol("AAPL");
        order.set_price(150.0 + i);
        order.set_quantity(100);
        order.set_side(Side::BUY);
        order.set_type(OrderType::LIMIT);
        order.set_timestamp(NetworkLib::NowUs());

        StreamEnvelope env;
        env.mutable_header()->set_message_type("ORDER_NEW");
        env.mutable_payload()->set_data(order.SerializeAsString());

        if (client->Send(env)) {
            LOG_INFO("Sent Order %d", i);
        } else {
            LOG_ERROR("Failed to send order %d", i);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
