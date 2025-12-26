#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "networklib/network_lib.h"
#include "networklib/core/event/reactor.h"
#include "networklib/protocols/tcp/tcp_handler.h"
#include "networklib/logger.h"

using namespace networklib;

class EchoHandler : public protocols::tcp::TcpHandler {
public:
    void OnConnection(const core::Connection::Ptr& /*conn*/) override {
        // No-op
    }

    void OnMessage(const core::Connection::Ptr& conn) override {
        auto& buffer = conn->InputBuffer();
        // Echo back
        std::string data(buffer.Peek(), buffer.ReadableBytes());
        buffer.RetrieveAll();
        conn->Send(data);
    }
};

void run_server(int port, std::atomic<bool>& ready) {
    logging::Logger::Initialize("appname=PerfServer;console=false;severity=Error");

    auto loop = std::make_unique<core::event::EventLoop>();
    loop->Init();
    core::event::Reactor reactor(std::move(loop));

    auto handler = std::make_shared<EchoHandler>();
    reactor.RegisterServer(port, handler);

    ready = true;
    reactor.Run();
}

void run_client(int port, int msg_count, int msg_size, std::atomic<long>& total_latency_us) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return;
    }

    std::string msg(msg_size, 'X');
    char buffer[1024];

    for (int i = 0; i < msg_count; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        send(sock, msg.c_str(), msg.size(), 0);

        int total_read = 0;
        while (total_read < msg_size) {
            int n = read(sock, buffer, sizeof(buffer));
            if (n > 0) total_read += n;
            else break;
        }
        auto end = std::chrono::high_resolution_clock::now();
        total_latency_us += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    }
    close(sock);
}

int main(int /*argc*/, char** /*argv*/) {
    int port = 9999;
    int msg_count = 10000;
    int msg_size = 128;
    int client_threads = 1;

    std::atomic<bool> server_ready(false);
    std::thread server_thread([port, &server_ready]() { run_server(port, server_ready); });

    while (!server_ready) std::this_thread::yield();

    std::cout << "Starting Performance Test..." << std::endl;
    std::cout << "Messages: " << msg_count << " per thread" << std::endl;
    std::cout << "Size: " << msg_size << " bytes" << std::endl;
    std::cout << "Threads: " << client_threads << std::endl;

    std::atomic<long> total_latency_us(0);
    auto start_time = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    for (int i = 0; i < client_threads; ++i) {
        threads.emplace_back(run_client, port, msg_count, msg_size, std::ref(total_latency_us));
    }

    for (auto& t : threads) t.join();

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    long total_msgs = msg_count * client_threads;
    double rps = (total_msgs * 1000.0) / duration_ms;
    double avg_latency = (double)total_latency_us / total_msgs;

    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "Total Time: " << duration_ms << " ms" << std::endl;
    std::cout << "Throughput: " << rps << " req/sec" << std::endl;
    std::cout << "Avg Latency: " << avg_latency << " us" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;

    // Force exit since server loop is infinite
    std::terminate();
    return 0;
}
