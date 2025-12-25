#ifndef NETWORKLIB_PROTOCOLS_WEBSOCKET_HANDSHAKE_H
#define NETWORKLIB_PROTOCOLS_WEBSOCKET_HANDSHAKE_H

#include <string>
#include <map>
#include "networklib/utils/result.h"

namespace networklib {
namespace protocols {
namespace websocket {

class Handshake {
public:
    /**
     * @brief Validates a WebSocket upgrade request and generates the response key.
     * 
     * @param headers HTTP headers from the request.
     * @return Result<std::string> The Sec-WebSocket-Accept value on success, or error.
     */
    static utils::Result<std::string> Process(const std::map<std::string, std::string>& headers);

    /**
     * @brief Generates the full HTTP 101 Switching Protocols response.
     */
    static std::string GenerateResponse(const std::string& accept_key);
};

} // namespace websocket
} // namespace protocols
} // namespace networklib

#endif // NETWORKLIB_PROTOCOLS_WEBSOCKET_HANDSHAKE_H
