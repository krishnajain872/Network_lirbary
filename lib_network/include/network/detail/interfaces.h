#ifndef NETWORKLIB_DETAIL_INTERFACES_H
#define NETWORKLIB_DETAIL_INTERFACES_H

#include <memory>
#include <string>
#include <functional>

namespace networklib {

    class StreamEnvelope;

    class IStreamContext {
    public:
        virtual ~IStreamContext() = default;
        virtual void Write(const StreamEnvelope& msg) = 0;
        virtual void Write(const std::string& data) = 0;
        virtual void Close() = 0;
    };

    class IServer {
    public:
        virtual ~IServer() = default;
        virtual bool Start() = 0;
        virtual void Stop() = 0;
        virtual void Wait() = 0;

        using StreamHandler = std::function<void(const StreamEnvelope& req, StreamEnvelope& resp, std::shared_ptr<IStreamContext> ctx)>;
        virtual void RegisterStreamHandler(StreamHandler handler) = 0;

        using RawHandler = std::function<void(const std::vector<char>& data, std::shared_ptr<IStreamContext> ctx)>;
        virtual void RegisterRawHandler(RawHandler handler) = 0;
    };

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

} // namespace networklib

#endif // NETWORKLIB_DETAIL_INTERFACES_H
