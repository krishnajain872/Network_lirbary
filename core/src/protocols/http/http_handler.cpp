#include "networklib/protocols/http/http_handler.h"
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
        // Handle Request
        std::string body = "Hello from NetworkLib HTTP Server!\nPath: " + ctx->path + "\n";
        SendResponse(conn, 200, body);
        
        // Reset for keep-alive or close if not
        // For simple phase, close after response (HTTP/1.0 style)
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
                    ctx.state = HttpContext::kDone; // Skip body for GET
                    return true;
                }
                
                // Parse Header
                size_t colon = line.find(':');
                if (colon != std::string::npos) {
                    // Safety check for empty value
                    if (colon + 2 <= line.length()) {
                        ctx.headers[line.substr(0, colon)] = line.substr(colon + 2);
                    } else {
                        ctx.headers[line.substr(0, colon)] = "";
                    }
                }
            } else {
                return false; // Need more data
            }
        }
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
