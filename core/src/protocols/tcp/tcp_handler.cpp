#include "networklib/protocols/tcp/tcp_handler.h"
#include "networklib/constants/limits.h"
#include <unistd.h>
#include <iostream>
#include <errno.h>

namespace networklib {
namespace protocols {
namespace tcp {

void TcpHandler::OnConnection(const core::Connection::Ptr& conn) {
    // std::cout << "New TCP connection: " << conn->Fd() << std::endl;
    (void)conn;
}

void TcpHandler::OnMessage(const core::Connection::Ptr& conn) {
    auto& input = conn->InputBuffer();
    // Echo back all readable bytes
    if (input.ReadableBytes() > 0) {
        conn->Send(input.Peek(), input.ReadableBytes());
        input.RetrieveAll();
    }
}

} // namespace tcp
} // namespace protocols
} // namespace networklib
