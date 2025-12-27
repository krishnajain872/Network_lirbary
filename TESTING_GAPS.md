# Testing Gaps

## Missing Tests

### 1. TCP/UDP Logic
- **Gap**: No tests exist that verify TCP/UDP actually process messages (because the implementation is broken).
- **Recommendation**: Once TCP/UDP handlers are fixed, add `basic_flow_test.cpp` variants for `mode: tcp` and `mode: udp`.

### 2. TLS Handshake (Client)
- **Gap**: No test verifies a secure connection between Client and Server.
- **Recommendation**: Add a test case with self-signed certificates enabling `ssl: true` in config. This would immediately expose the `TLS_server_method` issue in the Client.

### 3. Large Message Handling
- **Gap**: No tests for messages > 64KB (WebSocket limit issue).
- **Recommendation**: Add a test sending 1MB+ payload via WebSocket and gRPC to verify fragmentation and reassembly.

### 4. Concurrency & Stress
- **Gap**: Integration tests are single-threaded (one client).
- **Recommendation**: Add a load test spawning 100+ clients to verify `EventLoop` thread safety and `TokenBucket` behavior under load.

### 5. Failure Scenarios
- **Gap**: No tests for server crash, network partition, or malformed packets.
- **Recommendation**: Add "Fuzzing" tests sending garbage data to ports to ensure the Server doesn't crash (exception handling).
