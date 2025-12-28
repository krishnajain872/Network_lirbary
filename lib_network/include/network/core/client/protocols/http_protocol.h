#ifndef NETWORKLIB_CORE_CLIENT_PROTOCOLS_HTTP_PROTOCOL_H
#define NETWORKLIB_CORE_CLIENT_PROTOCOLS_HTTP_PROTOCOL_H

#include "network/core/client/client_protocol.h"
#include "stream_envelope.pb.h"
#include <sstream>

namespace networklib {
namespace core {
namespace client {

class HttpClientProtocol : public IClientProtocol {
public:
    void OnConnect(const std::shared_ptr<Connection>& /*conn*/) override {}

    void Send(const std::shared_ptr<Connection>& conn, const StreamEnvelope& envelope) override {
        std::string body;
        if (envelope.has_payload()) body = envelope.payload().data();

        std::string path = "/";
        if (envelope.has_header() && !envelope.header().message_type().empty()) {
            path = envelope.header().message_type();
        }

        std::stringstream ss;
        ss << "POST " << path << " HTTP/1.1\r\n";
        // Host should come from connection/config, but we don't have it here easily unless passed.
        // Assuming user puts Host in metadata? Or we hardcode "localhost" for now.
        ss << "Host: localhost\r\n";

        // Metadata to Headers
        if (envelope.has_metadata()) {
            for (const auto& [k, v] : envelope.metadata().fields()) {
                ss << k << ": " << v << "\r\n";
            }
        }

        ss << "Content-Length: " << body.size() << "\r\n";
        ss << "Connection: keep-alive\r\n";
        ss << "\r\n";
        ss << body;

        conn->Send(ss.str());
    }

    void OnDataReceived(const std::shared_ptr<Connection>& conn, IClient::MessageHandler handler) override {
        // Reuse HTTP parser logic?
        // For Client, we expect Response.
        // Naive parser for now matching Server's logic.
        auto& buf = conn->InputBuffer();

        const char* crlf = buf.FindCRLF();
        if (crlf) {
            // Check status line "HTTP/1.1 200 OK"
            // ...
            // Simplify: Just find body.
            const char* body_start = strstr(buf.Peek(), "\r\n\r\n");
            if (body_start) {
                // Parse headers for Content-Length?
                // Assume Body is rest of buffer for this simplified client.
                std::string body(body_start + 4, buf.ReadableBytes() - (body_start - buf.Peek() + 4));

                StreamEnvelope env;
                env.mutable_payload()->set_data(body);
                env.mutable_header()->set_message_type("http.response");

                if (handler) handler(env);
                buf.RetrieveAll();
            }
        }
    }
};

}
}
}

#endif
