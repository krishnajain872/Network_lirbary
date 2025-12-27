#include "networklib/network_lib.h"
#include "orders.pb.h"
#include "stream_envelope.pb.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <atomic>
#include <numeric>
#include <algorithm>
#include <condition_variable>

using namespace networklib;
using namespace networklib::oms;

// Basic stats container
struct BenchmarkStats {
    std::vector<int64_t> latencies_us;
    std::atomic<int> sent_count{0};
    std::atomic<int> received_count{0};
    std::chrono::steady_clock::time_point start_time;
    std::chrono::steady_clock::time_point end_time;
};

void RunBenchmarkScenario(std::shared_ptr<IClient> client) {
    std::cout << "Starting Benchmark Scenario..." << std::endl;
    LOG_INFO("Starting Benchmark Scenario...");

    // Configuration
    const int TOTAL_MESSAGES = 10000;
    const int WARMUP_MESSAGES = 100;

    BenchmarkStats stats;
    stats.latencies_us.reserve(TOTAL_MESSAGES);

    std::mutex stats_mutex;
    std::condition_variable cv;
    bool finished = false;

    // Register handler to measure latency
    client->RegisterMessageHandler([&](const StreamEnvelope& msg) {
        if (msg.header().message_type() == "ORDER_RESPONSE") {
            auto now = std::chrono::steady_clock::now();
            // Assuming we receive responses in order or we can just measure RTT from send time if we tracked it.
            // But since we are pumping messages, mapping request to response is hard without an ID map.
            // Simpler approach: Include send timestamp in the order payload or header.

            // Let's rely on the order ID to store timestamp if possible, or just the header metadata.
            // StreamEnvelope has metadata map? No.
            // Use Client Order ID?

            OrderResponse resp;
            if (resp.ParseFromString(msg.payload().data())) {
                std::string cl_ord_id = resp.client_order_id();
                try {
                    long long send_time_us = std::stoll(cl_ord_id);
                    auto now_us = std::chrono::duration_cast<std::chrono::microseconds>(
                        now.time_since_epoch()).count();
                    long long latency = now_us - send_time_us;

                    std::lock_guard<std::mutex> lock(stats_mutex);
                    if (stats.received_count < TOTAL_MESSAGES) {
                        stats.latencies_us.push_back(latency);
                    }
                    stats.received_count++;

                    if (stats.received_count >= TOTAL_MESSAGES) {
                        stats.end_time = now;
                        finished = true;
                        cv.notify_one();
                    }
                } catch (...) {
                    // Ignore parsing errors
                }
            }
        }
    });

    // Warmup
    std::cout << "Warming up..." << std::endl;
    LOG_INFO("Warming up...");
    for (int i = 0; i < WARMUP_MESSAGES; ++i) {
        Order order;
        order.set_order_id("WARM-" + std::to_string(i));
        order.set_symbol("AAPL");
        order.set_price(100.0);
        order.set_quantity(1);
        order.set_side(Side::BUY);
        order.set_type(OrderType::LIMIT);

        StreamEnvelope env;
        env.mutable_header()->set_message_type("ORDER_NEW");
        env.mutable_payload()->set_data(order.SerializeAsString());
        client->Send(env);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    std::this_thread::sleep_for(std::chrono::seconds(1)); // Let warmup finish
    stats.received_count = 0; // Reset for actual test
    stats.latencies_us.clear();

    std::cout << "Running Benchmark (" << TOTAL_MESSAGES << " messages)..." << std::endl;
    LOG_INFO("Running Benchmark (%d messages)...", TOTAL_MESSAGES);
    stats.start_time = std::chrono::steady_clock::now();

    for (int i = 0; i < TOTAL_MESSAGES; ++i) {
        auto now = std::chrono::steady_clock::now();
        auto now_us = std::chrono::duration_cast<std::chrono::microseconds>(
            now.time_since_epoch()).count();

        Order order;
        order.set_order_id("BENCH-" + std::to_string(i));
        order.set_client_order_id(std::to_string(now_us)); // Embed timestamp
        order.set_symbol("AAPL");
        order.set_price(150.0);
        order.set_quantity(100);
        order.set_side(Side::BUY);
        order.set_type(OrderType::LIMIT);

        StreamEnvelope env;
        env.mutable_header()->set_message_type("ORDER_NEW");
        env.mutable_payload()->set_data(order.SerializeAsString());

        client->Send(env);
        stats.sent_count++;

        // Don't flood too hard if synchronous, but async send is fine.
        // Maybe yield every now and then to let IO thread breathe?
        if (i % 100 == 0) std::this_thread::yield();
    }

    // Wait for completion
    std::unique_lock<std::mutex> lock(stats_mutex);
    if (cv.wait_for(lock, std::chrono::seconds(30), [&]{ return finished; })) {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            stats.end_time - stats.start_time).count();

        double seconds = duration / 1000000.0;
        double tps = TOTAL_MESSAGES / seconds;

        // Calculate latency stats
        std::sort(stats.latencies_us.begin(), stats.latencies_us.end());
        long long min_lat = stats.latencies_us.front();
        long long max_lat = stats.latencies_us.back();
        long long avg_lat = std::accumulate(stats.latencies_us.begin(), stats.latencies_us.end(), 0LL) / stats.latencies_us.size();
        long long p50 = stats.latencies_us[stats.latencies_us.size() * 0.50];
        long long p99 = stats.latencies_us[stats.latencies_us.size() * 0.99];

        std::cout << "\n=== Benchmark Results ===\n";
        std::cout << "Total Messages: " << TOTAL_MESSAGES << "\n";
        std::cout << "Total Time:     " << seconds << " s\n";
        std::cout << "Throughput:     " << tps << " msgs/sec\n";
        std::cout << "Latency (us):   Min=" << min_lat << ", Max=" << max_lat << ", Avg=" << avg_lat << "\n";
        std::cout << "                P50=" << p50 << ", P99=" << p99 << "\n";
        std::cout << "=========================\n";
    } else {
        LOG_ERROR("Benchmark timed out. Received %d/%d", stats.received_count.load(), TOTAL_MESSAGES);
    }
}
