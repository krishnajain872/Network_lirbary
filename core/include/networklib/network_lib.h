#ifndef NETWORKLIB_NETWORK_LIB_H
#define NETWORKLIB_NETWORK_LIB_H

#include <memory>
#include <string>
#include <functional>
#include <any>

namespace networklib {
    namespace config {
        struct ServerConfig;
        struct ClientConfig;
    }
    class StreamEnvelope; // Forward declaration

    /**
     * @brief Interface for a Network Server.
     */
    class IServer {
    public:
        virtual ~IServer() = default;

        /**
         * @brief Starts the server (non-blocking).
         * @return true if started successfully.
         */
        virtual bool Start() = 0;

        /**
         * @brief Stops the server.
         */
        virtual void Stop() = 0;

        /**
         * @brief Blocks until the server stops.
         */
        virtual void Wait() = 0;

        // Optional StreamHandler support
        // Note: The user must include "networklib/protocols/proto/stream_envelope.pb.h" to use this.
        using StreamHandler = std::function<void(const StreamEnvelope& req, StreamEnvelope& resp)>;

        /**
         * @brief Registers a generic StreamEnvelope handler.
         * The library will adapt incoming protocol messages (HTTP, gRPC, TCP) to StreamEnvelope.
         */
        virtual void RegisterStreamHandler(StreamHandler handler) = 0;

        // Raw Handler support can be added via specific ProtocolHandler registration
        // e.g., RegisterProtocolHandler(...)
    };

    /**
     * @brief Interface for a Network Client.
     */
    class IClient {
    public:
        virtual ~IClient() = default;

        virtual bool Connect() = 0;
        virtual void Disconnect() = 0;

        // Send generic message (raw bytes)
        virtual bool Send(const std::string& data) = 0;

        // Send StreamEnvelope (if supported)
        // Note: Requires "networklib/protocols/proto/stream_envelope.pb.h"
        virtual bool Send(const StreamEnvelope& envelope) = 0;
    };

    /**
     * @brief Main Entry Point for the Library.
     */
    class NetworkLib {
    public:
        /**
         * @brief Creates a Server instance based on configuration.
         * @param config The server configuration.
         * @return A shared pointer to the server interface.
         */
        static std::shared_ptr<IServer> CreateServer(const config::ServerConfig& config);

        /**
         * @brief Creates a Server instance from a config file.
         * @param config_path Path to the configuration file.
         * @return A shared pointer to the server interface.
         */
        static std::shared_ptr<IServer> CreateServer(const std::string& config_path);

        /**
         * @brief Creates a Client instance from a config file.
         */
        static std::shared_ptr<IClient> CreateClient(const std::string& config_path);

        /**
         * @brief Initialize the global logger.
         * @param config_string Semicolon-separated config string (e.g., "appname=MyApp;console=true").
         * @return true if initialized successfully.
         */
        static bool InitializeLogger(const char* config_string);
    };

} // namespace networklib

#endif // NETWORKLIB_NETWORK_LIB_H
