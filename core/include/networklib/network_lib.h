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
    class StreamEnvelope;

    /**
     * @brief Context for a stream, allowing asynchronous writes and state management.
     */
    class IStreamContext {
    public:
        virtual ~IStreamContext() = default;

        /**
         * @brief Write a message to the stream.
         * @param msg The message to send.
         */
        virtual void Write(const StreamEnvelope& msg) = 0;

        /**
         * @brief Close the stream.
         */
        virtual void Close() = 0;

        // TODO: Access Connection ID, Peer Info, etc.
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

        // Handler receives Request, Response (mutable), and Context (for streaming/async)
        // If response is populated, it is sent after return.
        // Context can be used for later writes (e.g. subscriptions).
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

    /**
     * @brief Main Entry Point for the Library.
     */
    class NetworkLib {
    public:
        static std::shared_ptr<IServer> CreateServer(const config::ServerConfig& config);
        static std::shared_ptr<IServer> CreateServer(const std::string& config_path);
        static std::shared_ptr<IClient> CreateClient(const std::string& config_path);
    };

} // namespace networklib

#endif // NETWORKLIB_NETWORK_LIB_H
