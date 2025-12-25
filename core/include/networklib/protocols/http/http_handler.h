#ifndef NETWORKLIB_PROTOCOLS_HTTP_HTTP_HANDLER_H
#define NETWORKLIB_PROTOCOLS_HTTP_HTTP_HANDLER_H

#include "networklib/protocols/protocol_handler.h"
#include <string>
#include <map>

namespace networklib {
namespace protocols {
namespace http {

// A very simple HTTP parser state for demonstration
struct HttpContext {
    enum State { kExpectRequestLine, kExpectHeaders, kExpectBody, kDone };
    State state = kExpectRequestLine;
    
    std::string method;
    std::string path;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;
    
    void Reset() {
        state = kExpectRequestLine;
        method.clear();
        path.clear();
        version.clear();
        headers.clear();
        body.clear();
    }
};

class HttpHandler : public ProtocolHandler {
public:
    void OnConnection(const networklib::core::Connection::Ptr& conn) override;
    void OnMessage(const networklib::core::Connection::Ptr& conn) override;

private:
    bool ParseRequest(networklib::core::memory::Buffer& buf, HttpContext& ctx);
    void SendResponse(const networklib::core::Connection::Ptr& conn, int status, const std::string& msg);
};

} // namespace http
} // namespace protocols
} // namespace networklib

#endif // NETWORKLIB_PROTOCOLS_HTTP_HTTP_HANDLER_H
