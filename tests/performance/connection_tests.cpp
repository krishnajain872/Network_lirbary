#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <string>

// A simple high-load simulator to be run by the user.
// It attempts to open thousands of connections to the server.

constexpr int TARGET_PORT = 8080;
constexpr int NUM_CONNECTIONS = 1000; // Start small for sandbox, user can increase

void connect_worker(int id, int count, std::atomic<int>& connected_count) {
    for (int i = 0; i < count; ++i) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) continue;

        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(TARGET_PORT);
        inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            close(sock);
        } else {
            connected_count++;
            // Keep connection open for a bit
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            close(sock);
        }
    }
}

int main(int argc, char** argv) {
    int connections = NUM_CONNECTIONS;
    if (argc > 1) connections = std::stoi(argv[1]);

    std::cout << "Starting load test with " << connections << " connections..." << std::endl;

    std::atomic<int> connected_count{0};
    int threads = 10;
    int conns_per_thread = connections / threads;

    std::vector<std::thread> pool;
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < threads; ++i) {
        pool.emplace_back(connect_worker, i, conns_per_thread, std::ref(connected_count));
    }

    for (auto& t : pool) t.join();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;

    std::cout << "Finished. Successful connections: " << connected_count << std::endl;
    std::cout << "Time taken: " << diff.count() << "s" << std::endl;
    std::cout << "Rate: " << connected_count / diff.count() << " conn/s" << std::endl;

    return 0;
}
