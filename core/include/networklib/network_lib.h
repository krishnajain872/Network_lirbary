#ifndef NETWORKLIB_NETWORK_LIB_H
#define NETWORKLIB_NETWORK_LIB_H

#include <memory>
#include <string>
#include <functional>
#include <vector>
#include <any>
#include <iostream>
#include <cstring>
#include <cstdarg>

namespace networklib {

    // Forward declaration of StreamEnvelope to avoid Proto dependency in header
    class StreamEnvelope;

    // ========================================================================
    // LOGGING API
    // ========================================================================
    namespace logging {
        enum class LogLevel {
            Trace,
            Debug,
            Info,
            Warn,
            Error,
            Fatal,
            Critical
        };

        class Logger {
        public:
            // Initialize with semicolon-separated config string
            static bool Initialize(const char* config_string);

            // Main logging function - uses default logger
            static void Log(LogLevel level, const char* file, int line, const char* func, const char* format, ...);

            // Log with explicit app name
            static void LogApp(const char* app_name, LogLevel level, const char* file, int line, const char* func, const char* format, ...);

            // Cleanup and flush
            static void Deinitialize();

            // Check if a level is enabled (for fast skipping)
            static bool IsLevelEnabled(LogLevel level);
        };
    }

    // Public Macros
    #define LOG_TRACE(fmt, ...) networklib::logging::Logger::Log(networklib::logging::LogLevel::Trace, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
    #define LOG_DEBUG(fmt, ...) networklib::logging::Logger::Log(networklib::logging::LogLevel::Debug, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
    #define LOG_INFO(fmt, ...)  networklib::logging::Logger::Log(networklib::logging::LogLevel::Info, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
    #define LOG_WARN(fmt, ...)  networklib::logging::Logger::Log(networklib::logging::LogLevel::Warn, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
    #define LOG_ERROR(fmt, ...) networklib::logging::Logger::Log(networklib::logging::LogLevel::Error, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

    // Internal Macros (preserved for backward compatibility during refactor)
    #define LOG(level, fmt, ...) \
        do { \
            if ((fmt) && \
                networklib::logging::Logger::IsLevelEnabled(networklib::logging::LogLevel::level)) { \
                _Pragma("GCC diagnostic push") \
                _Pragma("GCC diagnostic ignored \"-Wformat-security\"") \
                networklib::logging::Logger::Log(networklib::logging::LogLevel::level, \
                    __FILE__, __LINE__, __FUNCTION__, (fmt), ##__VA_ARGS__); \
                _Pragma("GCC diagnostic pop") \
            } \
        } while(0)

    #define LOG_APP(app_name, level, fmt, ...) \
        do { \
            if ((fmt)) { \
                _Pragma("GCC diagnostic push") \
                _Pragma("GCC diagnostic ignored \"-Wformat-security\"") \
                networklib::logging::Logger::LogApp((app_name), networklib::logging::LogLevel::level, \
                    __FILE__, __LINE__, __FUNCTION__, (fmt), ##__VA_ARGS__); \
                _Pragma("GCC diagnostic pop") \
            } \
        } while(0)


    // ========================================================================
    // INTERFACES
    // ========================================================================

    /**
     * @brief Context for a stream, allowing asynchronous writes and state management.
     */
    class IStreamContext {
    public:
        virtual ~IStreamContext() = default;
        virtual void Write(const StreamEnvelope& msg) = 0;
        virtual void Close() = 0;
    };

    /**
     * @brief Interface for a Network Server.
     */
    class IServer {
    public:
        virtual ~IServer() = default;

        virtual bool Start() = 0;
        virtual void Stop() = 0;
        virtual void Wait() = 0;

        using StreamHandler = std::function<void(const StreamEnvelope& req, StreamEnvelope& resp, std::shared_ptr<IStreamContext> ctx)>;
        virtual void RegisterStreamHandler(StreamHandler handler) = 0;
    };

    /**
     * @brief Interface for a Network Client.
     */
    class IClient {
    public:
        virtual ~IClient() = default;

        virtual bool Connect() = 0;
        virtual void Disconnect() = 0;
        virtual bool Send(const std::string& data) = 0;
        virtual bool Send(const StreamEnvelope& envelope) = 0;

        using MessageHandler = std::function<void(const StreamEnvelope&)>;
        virtual void RegisterMessageHandler(MessageHandler handler) = 0;
    };

    // ========================================================================
    // FACTORY
    // ========================================================================
    class NetworkLib {
    public:
        /**
         * @brief Create a Server instance based on the YAML configuration file.
         * The configuration determines the protocol (grpc, http, tcp, etc.) and settings.
         */
        static std::shared_ptr<IServer> CreateServer(const std::string& config_path);

        /**
         * @brief Create a Client instance based on the YAML configuration file.
         */
        static std::shared_ptr<IClient> CreateClient(const std::string& config_path);

        // Helper to get current timestamp
        static int64_t NowUs();
    };

} // namespace networklib

#endif // NETWORKLIB_NETWORK_LIB_H
