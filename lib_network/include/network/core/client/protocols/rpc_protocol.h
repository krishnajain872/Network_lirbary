#ifndef NETWORKLIB_CORE_CLIENT_PROTOCOLS_RPC_PROTOCOL_H
#define NETWORKLIB_CORE_CLIENT_PROTOCOLS_RPC_PROTOCOL_H

#include "network/core/client/protocols/tcp_protocol.h"

namespace networklib {
namespace core {
namespace client {

// RPC uses same framing as TCP (4-byte length prefix)
using RpcClientProtocol = TcpClientProtocol;

}
}
}

#endif
