#include "networklib/protocols/websocket/websocket_handler.h"
#include "networklib/protocols/websocket/handshake.h"
#include "networklib/core/memory/buffer.h"
#include "networklib/logger.h"
#include <iostream>
#include <map>
#include <sstream>

namespace networklib {
namespace protocols {
namespace websocket {

void WebSocketHandler::OnConnection(const core::Connection::Ptr& conn) {
    // Initial state: Expect HTTP Upgrade
    is_upgraded_ = false;
}

void WebSocketHandler::OnMessage(const core::Connection::Ptr& conn) {
    auto& buf = conn->InputBuffer();

    if (!is_upgraded_) {
        // Simple HTTP Handshake parsing
        // Find double CRLF
        const char* end = strstr(buf.Peek(), "\r\n\r\n");
        if (end) {
            std::string header_str(buf.Peek(), end);
            buf.Retrieve(header_str.size() + 4);

            // Parse Headers into map
            std::map<std::string, std::string> headers;
            std::istringstream stream(header_str);
            std::string line;
            while (std::getline(stream, line) && line != "\r") {
                size_t colon = line.find(':');
                if (colon != std::string::npos) {
                    // Trim space
                    std::string key = line.substr(0, colon);
                    std::string val = line.substr(colon + 2); // +2 for ": "
                    if (!val.empty() && val.back() == '\r') val.pop_back();
                    headers[key] = val;
                }
            }

            auto res = Handshake::Process(headers);
            if (res) {
                std::string response = Handshake::GenerateResponse(res.Value());
                conn->Send(response);
                is_upgraded_ = true;
                logging::Logger::Log(logging::LogLevel::Info, __FILE__, __LINE__, __FUNCTION__, "WebSocket Upgraded!");
            } else {
                conn->Send("HTTP/1.1 400 Bad Request\r\n\r\n");
                conn->ForceClose();
            }
        }
        return;
    }

    // WebSocket Frame Parsing
    Frame frame;
    size_t consumed = 0;
    
    // Loop to handle multiple frames in buffer
    while (buf.ReadableBytes() > 0) {
        if (parser_.Parse(buf.Peek(), buf.ReadableBytes(), consumed, frame)) {
            buf.Retrieve(consumed);
            
            if (frame.opcode == OpCode::kText) {
                std::string msg(frame.payload.begin(), frame.payload.end());
                logging::Logger::Log(logging::LogLevel::Info, __FILE__, __LINE__, __FUNCTION__, "WS Recv: %s", msg.c_str());
                
                // Echo back (masked=false for server-to-client)
                // Need a Frame encoder. 
                // For Phase 16, manual construct:
                // Fin=1, Op=1, Mask=0, Len=msg.size()
                std::vector<char> resp;
                resp.push_back(0x81); // Fin | Text
                resp.push_back(static_cast<uint8_t>(msg.size())); // Assumes < 126
                resp.insert(resp.end(), msg.begin(), msg.end());
                
                conn->Send(std::string(resp.begin(), resp.end()));
            } else if (frame.opcode == OpCode::kClose) {
                conn->ForceClose();
                return;
            }
        } else {
            break; // Wait for more data
        }
    }
}

} // namespace websocket
} // namespace protocols
} // namespace networklib
