#include "networklib/protocols/udp/udp_handler.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include "stream_envelope.pb.h"
#include "networklib/logging.h"

namespace networklib {
namespace protocols {
namespace udp {

class UdpStreamContext : public networklib::IStreamContext {
public:
    UdpStreamContext(int fd, const std::string& ip, int port)
        : fd_(fd), ip_(ip), port_(port) {}

    void Write(const StreamEnvelope& msg) override {
        std::string payload = msg.SerializeAsString();

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port_);
        inet_pton(AF_INET, ip_.c_str(), &addr.sin_addr);

        sendto(fd_, payload.data(), payload.size(), 0, (struct sockaddr*)&addr, sizeof(addr));
    }
    
    void Close() override {} // UDP is connectionless

private:
    int fd_;
    std::string ip_;
    int port_;
};

void EchoUdpHandler::OnPacket(int fd, const char* data, size_t len, const std::string& source_ip, int source_port) {
    if (stream_handler_) {
        StreamEnvelope req;
        // Assume payload is serialized envelope OR raw bytes
        // Try parsing
        if (!req.ParseFromArray(data, len)) {
            // Fallback: Raw bytes
            req.mutable_payload()->set_data(std::string(data, len));
            req.mutable_header()->set_message_type("udp.packet");
        }

        StreamEnvelope resp;
        auto ctx = std::make_shared<UdpStreamContext>(fd, source_ip, source_port);

        try {
            stream_handler_(req, resp, ctx);
            if (resp.has_payload()) {
                ctx->Write(resp);
            }
        } catch (...) {}
    } else {
        // Echo
        struct sockaddr_in cliaddr;
        cliaddr.sin_family = AF_INET;
        cliaddr.sin_port = htons(source_port);
        inet_pton(AF_INET, source_ip.c_str(), &cliaddr.sin_addr);

        sendto(fd, data, len, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
    }
}

} // namespace udp
} // namespace protocols
} // namespace networklib
