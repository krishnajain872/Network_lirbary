#include "networklib/protocols/http/http_handler.h"
#include "stream_envelope.pb.h"
#include <sstream>
#include <iostream>

namespace networklib {
namespace protocols {
namespace http {

void HttpHandler::OnConnection(const core::Connection::Ptr& conn) {
    // Initialize context
    conn->SetContext(HttpContext());
}

void HttpHandler::OnMessage(const core::Connection::Ptr& conn) {
    auto& buf = conn->InputBuffer();
    HttpContext* ctx = std::any_cast<HttpContext>(conn->GetMutableContext());
    if (!ctx) return;

    if (ParseRequest(buf, *ctx)) {
        if (stream_handler_) {
            // Convert HTTP to StreamEnvelope
            networklib::StreamEnvelope req_env;

            // Map Method/Path to Message Type (e.g. "GET /api/foo")
            req_env.mutable_header()->set_message_type(ctx->method + " " + ctx->path);
            req_env.mutable_header()->set_protocol_version(ctx->version);

            // Map Headers to Metadata
            auto* meta_fields = req_env.mutable_metadata()->mutable_fields();
            for (const auto& kv : ctx->headers) {
                (*meta_fields)[kv.first] = kv.second;
            }

            // Map Body
            if (!ctx->body.empty()) {
                req_env.mutable_payload()->set_data(ctx->body);
                req_env.mutable_payload()->set_size_bytes(ctx->body.size());
            }

            networklib::StreamEnvelope resp_env;
            try {
                // Invoke Application Callback
                stream_handler_(req_env, resp_env);

                // Convert Response StreamEnvelope to HTTP
                std::string resp_body;
                if (resp_env.has_payload()) {
                    resp_body = resp_env.payload().data();
                }

                // Determine Status Code (default 200, check metadata)
                int status = 200;
                auto& resp_meta = resp_env.metadata().fields();
                if (resp_meta.count("http_status")) {
                    status = std::stoi(resp_meta.at("http_status"));
                }

                SendResponse(conn, status, resp_body);

            } catch (const std::exception& e) {
                SendResponse(conn, 500, "Internal Server Error: " + std::string(e.what()));
            }
        } else {
            // Legacy/Raw Mode: Echo
            std::string body = "Hello from NetworkLib HTTP Server!\nPath: " + ctx->path + "\n";
            SendResponse(conn, 200, body);
        }
        
        // Reset context or close
        // Simple implementation: close after response
        conn->Shutdown();
    }
}

bool HttpHandler::ParseRequest(core::memory::Buffer& buf, HttpContext& ctx) {
    // Very naive parser for demonstration
    if (ctx.state == HttpContext::kExpectRequestLine) {
        const char* crlf = buf.FindCRLF();
        if (crlf) {
            std::string line(buf.Peek(), crlf);
            buf.Retrieve(line.size() + 2); // +2 for CRLF
            
            std::istringstream iss(line);
            iss >> ctx.method >> ctx.path >> ctx.version;
            ctx.state = HttpContext::kExpectHeaders;
        } else {
            return false; // Need more data
        }
    }

    if (ctx.state == HttpContext::kExpectHeaders) {
        while (true) {
            const char* crlf = buf.FindCRLF();
            if (crlf) {
                std::string line(buf.Peek(), crlf);
                buf.Retrieve(line.size() + 2);
                
                if (line.empty()) {
                    // End of headers
                    // If Content-Length exists, expect body
                    if (ctx.headers.count("Content-Length")) {
                         ctx.state = HttpContext::kExpectBody;
                         break;
                    }
                    ctx.state = HttpContext::kDone;
                    return true;
                }
                
                // Parse Header
                size_t colon = line.find(':');
                if (colon != std::string::npos) {
                    std::string key = line.substr(0, colon);
                    std::string val = line.substr(colon + 2); // Skip ": "
                    // Trim CR if present (FindCRLF handles it but let's be safe)
                    if (!val.empty() && val.back() == '\r') val.pop_back();
                    ctx.headers[key] = val;
                }
            } else {
                return false; // Need more data
            }
        }
    }
    
    if (ctx.state == HttpContext::kExpectBody) {
        // Naive: Read full body based on Content-Length
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
