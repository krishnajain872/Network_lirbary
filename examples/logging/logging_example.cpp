#include "networklib/logger.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace networklib::logging;

void RunWithConfig(const char* name, const char* config) {
    std::cout << "\n--- Running " << name << " ---" << std::endl;
    if (Logger::Initialize(config)) {
        LOG(Info, "Logger initialized with config: %s", config);
        LOG(Debug, "This is a debug message");
        LOG(Warn, "This is a warning message");
        LOG(Error, "This is an error message");

        // Simulate work
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        Logger::Deinitialize();
    } else {
        std::cerr << "Failed to initialize logger for " << name << std::endl;
    }
}

int main() {
    // 1. Console Debug
    RunWithConfig("Console Debug", "appname=Example1;console=true;severity=Debug");

    // 2. File Logging
    RunWithConfig("File Logging", "appname=Example2;logfile=example.log;console=true;severity=Info");

    // 3. Async Performance
    RunWithConfig("Async High Perf", "appname=Example3;console=false;async=true;async_queue_size=5000");

    // 4. JSON Format
    RunWithConfig("JSON Output", "appname=Example4;console=true;format=json;severity=Info");

    return 0;
}
