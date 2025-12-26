#include "networklib/protocols/websocket/websocket_handler.h"
#include "networklib/protocols/websocket/handshake.h"
#include "networklib/core/memory/buffer.h"
#include "networklib/logging.h"
#include "stream_envelope.pb.h"
#include <iostream>
#include <map>
#include <sstream>

namespace networklib {
namespace protocols {
namespace websocket {

// Generic Context Implementation
class WebSocketStreamContext : public networklib::IStreamContext {
public:
    WebSocketStreamContext(core::Connection::Ptr conn) : conn_(conn) {}

    void Write(const StreamEnvelope& msg) override {
        // Serialize msg to WS Frame (Text)
        std::string payload;
        if (msg.has_payload()) {
             payload = msg.payload().data();
        } else {
             // Maybe serialize whole envelope if binary?
             // For now assume payload = text
        }

        std::vector<char> out;
        out.push_back(0x81); // Fin | Text

        size_t len = payload.size();
        if (len < 126) {
            out.push_back(static_cast<uint8_t>(len));
        } else if (len < 65536) {
            out.push_back(126);
            out.push_back((len >> 8) & 0xFF);
            out.push_back(len & 0xFF);
        } else {
            out.push_back(127);
            for (int i = 7; i >= 0; --i) {
                out.push_back((len >> ((7 - i) * 8)) & 0xFF);
            }
        }

        out.insert(out.end(), payload.begin(), payload.end());
        conn_->Send(std::string(out.begin(), out.end()));
    }

    void Close() override { conn_->ForceClose(); }

private:
    core::Connection::Ptr conn_;
};

void WebSocketHandler::OnConnection(const core::Connection::Ptr& /*conn*/) {
    is_upgraded_ = false;
}

void WebSocketHandler::OnMessage(const core::Connection::Ptr& conn) {
    auto& buf = conn->InputBuffer();

    if (!is_upgraded_) {
        const char* end = strstr(buf.Peek(), "\r\n\r\n");
        if (end) {
            std::string header_str(buf.Peek(), end);
            buf.Retrieve(header_str.size() + 4);

            std::map<std::string, std::string> headers;
            std::istringstream stream(header_str);
            std::string line;
            while (std::getline(stream, line) && line != "\r") {
                size_t colon = line.find(':');
                if (colon != std::string::npos) {
                    std::string key = line.substr(0, colon);
                    std::string val = line.substr(colon + 2);
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

    Frame frame;
    size_t consumed = 0;
    
    while (buf.ReadableBytes() > 0) {
        if (parser_.Parse(buf.Peek(), buf.ReadableBytes(), consumed, frame)) {
            buf.Retrieve(consumed);
            
            if (frame.opcode == OpCode::kText || frame.opcode == OpCode::kBinary) {
                if (!CheckRateLimit()) {
                    conn->ForceClose(); // Or send Close frame with 1008
                    return;
                }
                if (stream_handler_) {
                    networklib::StreamEnvelope req;
                    req.mutable_header()->set_message_type("websocket");
                    req.mutable_payload()->set_data(std::string(frame.payload.begin(), frame.payload.end()));

                    networklib::StreamEnvelope resp;
                    auto ctx = std::make_shared<WebSocketStreamContext>(conn);

                    try {
                        stream_handler_(req, resp, ctx);

                        // If handler populated resp, send it immediately
                        // (Handler can also use ctx.Write later)
                        if (resp.has_payload()) {
                            ctx->Write(resp);
                        }
                    } catch (...) {}
                } else {
                    // Echo Legacy
                    std::string msg(frame.payload.begin(), frame.payload.end());
                    // ... echo logic ...
                }
            } else if (frame.opcode == OpCode::kClose) {
                conn->ForceClose();
                return;
            }
        } else {
            break;
        }
    }
}

} // namespace websocket
} // namespace protocols
} // namespace networklib
