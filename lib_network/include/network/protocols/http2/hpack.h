#ifndef NETWORKLIB_PROTOCOLS_HTTP2_HPACK_H
#define NETWORKLIB_PROTOCOLS_HTTP2_HPACK_H

#include <string>
#include <vector>
#include <map>
#include <cstdint>

namespace networklib {
namespace protocols {
namespace http2 {

class HpackEncoder {
public:
    static std::vector<uint8_t> Encode(const std::map<std::string, std::string>& headers);

private:
    static void EncodeInteger(std::vector<uint8_t>& out, uint32_t value, uint8_t n);
    static void EncodeString(std::vector<uint8_t>& out, const std::string& str);
};

} // namespace http2
} // namespace protocols
} // namespace networklib

#endif // NETWORKLIB_PROTOCOLS_HTTP2_HPACK_H
