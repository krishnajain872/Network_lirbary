#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

// Load Test Tool
// Simulates 10k clients connecting via TCP (mocking gRPC/HTTP)

void ClientWorker(int id, int count, std::atomic<int>& connected) {
    for (int i = 0; i < count; ++i) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) continue;

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(50051);
        inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

        if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
            connected++;
            // Keep open
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        close(sock);
    }
}

int main(int argc, char** argv) {
    int clients = 1000; // Default
    if (argc > 1) clients = std::stoi(argv[1]);

    std::cout << "Simulating " << clients << " clients..." << std::endl;
    std::atomic<int> connected{0};
    std::vector<std::thread> threads;
    int t_count = 10;
    
    for (int i = 0; i < t_count; ++i) {
        threads.emplace_back(ClientWorker, i, clients/t_count, std::ref(connected));
    }

    for (auto& t : threads) t.join();
    
    std::cout << "Connected: " << connected << std::endl;
    return 0;
}
