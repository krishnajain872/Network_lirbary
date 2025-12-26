#include "networklib/network_lib.h"
#include "market_data.pb.h"
#include "stream_envelope.pb.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace networklib;
using namespace networklib::oms;

void RunMarketDataConsumerScenario(std::shared_ptr<IClient> client) {
    LOG_INFO("Starting Market Data Consumer Scenario...");

    StreamEnvelope sub_env;
    sub_env.mutable_header()->set_message_type("MARKET_DATA_SUB");
    sub_env.mutable_payload()->set_data("AAPL"); // Simple payload

    client->Send(sub_env);

    std::this_thread::sleep_for(std::chrono::seconds(10));
}
