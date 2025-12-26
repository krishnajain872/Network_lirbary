#include "networklib/protocols/http/http_handler.h"
#include "networklib/logging.h"
#include "stream_envelope.pb.h"
#include <sstream>
#include <iostream>

namespace networklib {
namespace protocols {
namespace http {

class HttpStreamContext : public networklib::IStreamContext {
public:
    HttpStreamContext(core::Connection::Ptr conn) : conn_(conn), headers_sent_(false) {}

    void Write(const StreamEnvelope& msg) override {
        std::string body;
        if (msg.has_payload()) body = msg.payload().data();

        if (!headers_sent_) {
            std::stringstream ss;
            ss << "HTTP/1.1 200 OK\r\n";
            ss << "Connection: keep-alive\r\n";
            ss << "Transfer-Encoding: chunked\r\n";
            ss << "\r\n";
            conn_->Send(ss.str());
            headers_sent_ = true;
        }

        if (!body.empty()) {
            std::stringstream ss;
            ss << std::hex << body.size() << "\r\n";
            ss << body << "\r\n";
            conn_->Send(ss.str());
        }
    }

    void Close() override {
        if (headers_sent_) {
            conn_->Send("0\r\n\r\n");
        }
        conn_->Shutdown();
    }

private:
    core::Connection::Ptr conn_;
    bool headers_sent_;
};

void HttpHandler::OnConnection(const core::Connection::Ptr& conn) {
    conn->SetContext(HttpContext());
}

void HttpHandler::OnMessage(const core::Connection::Ptr& conn) {
    auto& buf = conn->InputBuffer();
    HttpContext* ctx = std::any_cast<HttpContext>(conn->GetMutableContext());
    if (!ctx) return;

    if (ParseRequest(buf, *ctx)) {
        if (!CheckRateLimit()) {
            SendResponse(conn, 429, "Too Many Requests");
            conn->Shutdown();
            return;
        }
        if (stream_handler_) {
            networklib::StreamEnvelope req_env;
            req_env.mutable_header()->set_message_type(ctx->method + " " + ctx->path);
            req_env.mutable_header()->set_protocol_version(ctx->version);

            auto* meta_fields = req_env.mutable_metadata()->mutable_fields();
            for (const auto& kv : ctx->headers) {
                (*meta_fields)[kv.first] = kv.second;
            }

            if (!ctx->body.empty()) {
                req_env.mutable_payload()->set_data(ctx->body);
            }

            networklib::StreamEnvelope resp_env;
            auto stream_ctx = std::make_shared<HttpStreamContext>(conn);

            try {
                stream_handler_(req_env, resp_env, stream_ctx);

                if (resp_env.has_payload()) {
                    stream_ctx->Write(resp_env);
                    stream_ctx->Close();
                }
            } catch (const std::exception& e) {
                SendResponse(conn, 500, "Internal Server Error");
            }
        } else {
            std::string body = "Hello from NetworkLib HTTP Server!\nPath: " + ctx->path + "\n";
            SendResponse(conn, 200, body);
        }
    }
}

bool HttpHandler::ParseRequest(core::memory::Buffer& buf, HttpContext& ctx) {
    // Same parser
    if (ctx.state == HttpContext::kExpectRequestLine) {
        const char* crlf = buf.FindCRLF();
        if (crlf) {
            std::string line(buf.Peek(), crlf);
            buf.Retrieve(line.size() + 2);
            std::istringstream iss(line);
            iss >> ctx.method >> ctx.path >> ctx.version;
            ctx.state = HttpContext::kExpectHeaders;
        } else {
            return false;
        }
    }

    if (ctx.state == HttpContext::kExpectHeaders) {
        while (true) {
            const char* crlf = buf.FindCRLF();
            if (crlf) {
                std::string line(buf.Peek(), crlf);
                buf.Retrieve(line.size() + 2);
                if (line.empty()) {
                    if (ctx.headers.count("Content-Length")) {
                         ctx.state = HttpContext::kExpectBody;
                         break;
                    }
                    ctx.state = HttpContext::kDone;
                    return true;
                }
                size_t colon = line.find(':');
                if (colon != std::string::npos) {
                    std::string key = line.substr(0, colon);
                    std::string val = line.substr(colon + 2);
                    if (!val.empty() && val.back() == '\r') val.pop_back();
                    ctx.headers[key] = val;
                }
            } else {
                return false;
            }
        }
    }
    
    if (ctx.state == HttpContext::kExpectBody) {
        size_t len = std::stoi(ctx.headers["Content-Length"]);
        if (buf.ReadableBytes() >= len) {
            ctx.body = std::string(buf.Peek(), len);
            buf.Retrieve(len);
            ctx.state = HttpContext::kDone;
            return true;
        }
        return false;
    }

    return ctx.state == HttpContext::kDone;
}

void HttpHandler::SendResponse(const core::Connection::Ptr& conn, int status, const std::string& msg) {
    std::stringstream ss;
    ss << "HTTP/1.1 " << status << " OK\r\n";
    ss << "Content-Type: text/plain\r\n";
    ss << "Content-Length: " << msg.size() << "\r\n";
    ss << "Connection: close\r\n";
    ss << "\r\n";
    ss << msg;
    conn->Send(ss.str());
}

} // namespace http
} // namespace protocols
} // namespace networklib
