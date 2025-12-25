#include "networklib/protocols/websocket/handshake.h"
#include "networklib/constants/errors.h"
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <cstring>
#include <iostream>

namespace networklib {
namespace protocols {
namespace websocket {

static const std::string kGuid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

// Helper for Base64 encoding
std::string Base64Encode(const unsigned char* data, size_t len) {
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); // No newlines
    BIO* bio = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bio);
    BIO_write(b64, data, len);
    BIO_flush(b64);
    BUF_MEM* bufferPtr;
    BIO_get_mem_ptr(b64, &bufferPtr);
    std::string res(bufferPtr->data, bufferPtr->length);
    BIO_free_all(b64);
    return res;
}

utils::Result<std::string> Handshake::Process(const std::map<std::string, std::string>& headers) {
    auto it_key = headers.find("Sec-WebSocket-Key");
    if (it_key == headers.end()) {
        return utils::Result<std::string>::Failure(
            constants::errors::kInvalidRequest, "Missing Sec-WebSocket-Key");
    }

    std::string key = it_key->second + kGuid;
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(key.c_str()), key.size(), hash);

    return utils::Result<std::string>::Success(Base64Encode(hash, SHA_DIGEST_LENGTH));
}

std::string Handshake::GenerateResponse(const std::string& accept_key) {
    return "HTTP/1.1 101 Switching Protocols\r\n"
           "Upgrade: websocket\r\n"
           "Connection: Upgrade\r\n"
           "Sec-WebSocket-Accept: " + accept_key + "\r\n\r\n";
}

} // namespace websocket
} // namespace protocols
} // namespace networklib
