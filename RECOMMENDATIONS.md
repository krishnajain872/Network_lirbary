# Recommendations

## Immediate Actions (Do First)

1.  **Fix TCP Handler**: Re-implement `TcpHandler` to use `StreamEnvelope` framing (e.g., 4-byte length prefix + Protobuf payload). This is the default mode and most users will expect it to work.
2.  **Fix TLS Context**: Modify `TlsContext` to support Client mode (`TLS_client_method`). This is a one-line change with high impact.
3.  **Merge Audit Fixes**: Commit the fixes for `EventLoop` race condition, `Client` connection logic, and `gRPC` hardcoding found during this audit.

## Short-term Actions (Next Sprint)

1.  **Implement UDP Logic**: Update `UdpHandler` to parse envelopes.
2.  **Complete WebSocket**: Implement 64-bit payload support.
3.  **Refactor ConfigParser**: Clarify `protocol` vs `mode` in YAML configuration to avoid confusion. Deprecate `server.mode` in favor of `server.protocols.<name>.enabled`.

## Long-term Actions (Technical Debt)

1.  **Real QUIC Implementation**: Replace the mock `QuicHandler` with a library like `ngtcp2`.
2.  **HTTP/2 Client Support**: `GrpcClientProtocol` currently manually constructs frames. Implement a proper `Http2ClientSession` to share logic with the Server.
3.  **Logging Overhaul**: Fix the file output logger integration and ensure log levels propagate correctly to all components.
