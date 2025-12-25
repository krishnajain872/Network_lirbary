# Architecture Overview

The Network Library is built on a **Reactor Pattern** utilizing non-blocking I/O.

## Core Components

### 1. Event Loop & Poller
- **Poller Interface**: Abstracts the underlying syscall mechanism.
- **EpollPoller**: Uses `epoll` (Linux standard).
- **IoUringPoller**: Uses `io_uring` (High performance, kernel 5.1+).
- **Selection**: Configured at runtime via YAML (`performance.event_loop_type`).

### 2. Reactor
- Manages the lifecycle of the Server Socket.
- Dispatches `Accept` events.
- Creates `Connection` objects and assigns them to the Event Loop.
- Wires up Security (DDoS, TLS) into the connection lifecycle.

### 3. Protocol Handlers
- **ProtocolFactory**: Creates handlers based on config string ("tcp", "http", "grpc", "quic").
- **Handlers**: State machines that parse raw bytes into protocol-specific messages.
- **StreamEnvelope**: The universal data carrier.

### 4. Zero-Code Switching
- Changing `server.mode` in YAML swaps the `ProtocolHandler` implementation transparently.
- Hot Reloading allows tuning (e.g. Rate Limits) without process restart.
