#include <gtest/gtest.h>
#include "networklib/logger.h"
#include <thread>
#include <chrono>
#include <fstream>
#include <filesystem>

using namespace networklib::logging;

TEST(LoggingTest, Initialization) {
    EXPECT_TRUE(Logger::Initialize("appname=TestApp;console=true;severity=Debug"));
    Logger::Deinitialize();
}

TEST(LoggingTest, FileLogging) {
    std::string log_file = "test_logging.log";
    if (std::filesystem::exists(log_file)) {
        std::filesystem::remove(log_file);
    }

    std::string config = "appname=FileTest;logfile=" + log_file + ";console=false;severity=Info";
    EXPECT_TRUE(Logger::Initialize(config.c_str()));

    LOG_INFO("This is an info message");
    LOG_WARN("This is a warning");
    LOG_DEBUG("This debug message should not appear");

    // Allow time for async flush
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    Logger::Deinitialize();

    // Verify file content
    std::ifstream file(log_file);
    ASSERT_TRUE(file.is_open());

    std::string line;
    std::vector<std::string> lines;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    EXPECT_GE(lines.size(), 2);
    EXPECT_NE(lines[0].find("This is an info message"), std::string::npos);
    EXPECT_NE(lines[1].find("This is a warning"), std::string::npos);
}

TEST(LoggingTest, ThreadSafety) {
    Logger::Initialize("appname=ThreadTest;console=false");

    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([i]() {
            for (int j = 0; j < 100; ++j) {
                LOG_INFO("Thread %d message %d", i, j);
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    Logger::Deinitialize();
}
