#include <gtest/gtest.h>
#include "logger/logging.h"
#include "logger/log_manager.h"
#include "logger/log_common.h"
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>

namespace fs = std::filesystem;

class LoggerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Clear old logs
        if (fs::exists("test_log.txt")) fs::remove("test_log.txt");
        if (fs::exists("test_log.txt.1")) fs::remove("test_log.txt.1");
        if (fs::exists("test_log.txt.2")) fs::remove("test_log.txt.2");
    }

    void TearDown() override {
        networklib::logging::Logger::Deinitialize();
        if (fs::exists("test_log.txt")) fs::remove("test_log.txt");
        if (fs::exists("test_log.txt.1")) fs::remove("test_log.txt.1");
        if (fs::exists("test_log.txt.2")) fs::remove("test_log.txt.2");
    }
};

TEST_F(LoggerTest, BasicLogging) {
    // Config: Console only, INFO level
    const char* config = "console=true;severity=INFO";
    ASSERT_TRUE(networklib::logging::Logger::Initialize(config));

    // Verify INFO is enabled, DEBUG is not
    EXPECT_TRUE(networklib::logging::Logger::IsLevelEnabled(networklib::logging::LogLevel::Info));
    EXPECT_FALSE(networklib::logging::Logger::IsLevelEnabled(networklib::logging::LogLevel::Debug));

    // We can't easily capture console output here without pipe redirection,
    // but we verify the API calls don't crash and flags are correct.
    LOG_INFO("This is an info message");
    LOG_DEBUG("This should not appear");
}

TEST_F(LoggerTest, FileLoggingAndRotation) {
    // Config: File enabled, small rotation size (100 bytes), max 2 backups
    // Note: buffer_size=0 to force frequent flushing for test? No, AsyncLogger has its own logic.
    // We'll rely on explicit Flush logic via shutdown or wait.
    std::string config_str = "logfile=test_log.txt;severity=INFO;rotation_size=50;max_backups=2;async=false";
    // Use async=false (sync) if supported? AsyncLogger is always async.
    // We will rely on Shutdown() to flush.

    ASSERT_TRUE(networklib::logging::Logger::Initialize(config_str.c_str()));

    // Write enough data to trigger rotation
    // Each log line has timestamp + metadata, likely > 50 bytes.
    for (int i = 0; i < 5; ++i) {
        LOG_INFO("Log message number %d which should be long enough to trigger rotation eventually", i);
        // Sleep slightly to ensure timestamps differ if needed, or just to let async thread catch up
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Force flush by shutting down (Deinitialize calls Shutdown)
    networklib::logging::Logger::Deinitialize();

    // Verify files exist
    // We expect test_log.txt and test_log.txt.1, maybe .2
    EXPECT_TRUE(fs::exists("test_log.txt"));
    // Since we wrote 5 times and limit is 50 bytes, we expect rotation.
    EXPECT_TRUE(fs::exists("test_log.txt.1"));
}

TEST_F(LoggerTest, LargeMessage) {
    const char* config = "logfile=test_log.txt;severity=INFO;async=false";
    ASSERT_TRUE(networklib::logging::Logger::Initialize(config));

    // Create a message larger than 4096 bytes
    std::string large_msg(5000, 'A');
    LOG_INFO("%s", large_msg.c_str());

    networklib::logging::Logger::Deinitialize();

    // Verify file contains the message
    std::ifstream ifs("test_log.txt");
    std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    EXPECT_NE(content.find(large_msg), std::string::npos);
}

TEST_F(LoggerTest, IsLevelEnabledDynamic) {
     const char* config = "severity=WARN";
     ASSERT_TRUE(networklib::logging::Logger::Initialize(config));

     EXPECT_FALSE(networklib::logging::Logger::IsLevelEnabled(networklib::logging::LogLevel::Info));
     EXPECT_TRUE(networklib::logging::Logger::IsLevelEnabled(networklib::logging::LogLevel::Warn));
     EXPECT_TRUE(networklib::logging::Logger::IsLevelEnabled(networklib::logging::LogLevel::Error));
}
