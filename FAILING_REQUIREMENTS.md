# Failing Requirements Analysis

## Critical Failures (Immediate Attention Required)

### [FR-001] TCP Protocol Support
- **Issue**: The `TcpHandler` is implemented as an "Echo Server". It reads bytes and immediately writes them back to the socket. It completely ignores the `StreamEnvelope` protobuf structure and never calls the user's registered `StreamHandler`.
- **Impact**: Any application built expecting TCP message processing will fail to function. The OMS Client/Server scenario (which defaults to TCP) is broken.
- **Code Location**: `core/src/protocols/tcp/tcp_handler.cpp`
- **Suggested Fix**: Implement a framing strategy (e.g., length-prefixed) to read full `StreamEnvelope` messages, parse them, and dispatch to `stream_handler_`.

### [FR-007] TLS Security (Client Side)
- **Issue**: The `TlsContext::Init` function hardcodes `SSL_CTX_new(TLS_server_method())`. This creates an SSL context compatible only with server-side handshakes.
- **Impact**: Clients trying to connect via SSL will likely fail the handshake or be rejected by the server.
- **Code Location**: `core/src/security/tls_context.cpp`
- **Suggested Fix**: Add a parameter to `Init` or `TlsContext` constructor to specify `SslMode` (Client vs Server) and use `TLS_client_method()` or `TLS_method()` accordingly.

## Major Issues (High Priority)

### [FR-002] UDP Protocol Support
- **Issue**: Similar to TCP, `UdpHandler` is an Echo server.
- **Impact**: UDP applications cannot process business logic.
- **Code Location**: `core/src/protocols/udp/udp_handler.cpp`
- **Suggested Fix**: Deserialize payload into `StreamEnvelope` (if fitting in packet) or define a UDP-specific framing/protocol.

### [FR-006] QUIC Protocol Support
- **Issue**: The implementation is a "Mock". It simulates streams using string parsing on top of a generic connection, without actual QUIC cryptographic handshake or flow control.
- **Impact**: Not production ready. Misleading if claimed as a feature.
- **Code Location**: `core/src/protocols/quic/quic_handler.cpp`
- **Suggested Fix**: Integrate `ngtcp2` or `OpenSSL QUIC` (if available) for real implementation.

## Minor Issues (Medium/Low Priority)

### [FR-005] WebSocket 64-bit Payloads
- **Issue**: Frame parser has a `TODO` for 64-bit length fields.
- **Impact**: Messages larger than 65535 bytes will be parsed incorrectly or dropped.
- **Code Location**: `core/src/protocols/websocket/frame_parser.cpp`
- **Suggested Fix**: Implement `ntohll` logic for 64-bit length parsing.
