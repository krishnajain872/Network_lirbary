# Network Library

A high-performance, configuration-driven C++20 network library designed for 10B+ connections.

## Features

*   **Zero-Code Switching:** Switch between TCP, UDP, HTTP, gRPC, WebSocket, and QUIC via YAML.
*   **High Performance:** Supports `epoll` and `io_uring` (Linux 5.1+).
*   **Production Grade:** Includes Hot Reload, DDoS Protection Stub, and detailed Metrics.
*   **Polyglot:** Python bindings included.
*   **Reference App:** Full-stack Order Management System (OMS) with 3D Web Frontend.

## Building

### Docker (Recommended)

```bash
cd docker
docker compose up --build -d
```

Access the Frontend at http://localhost:3000

### Manual Build

Requires: C++20, CMake, Ninja, Protobuf, OpenSSL, liburing, libgtest, pybind11.

```bash
mkdir build && cd build
cmake -G Ninja -DENABLE_TESTS=ON ..
cmake --build .
```

## Configuration

Edit `config/examples/grpc/grpc_server_bidirectional.yaml` to tune performance:

```yaml
server:
  performance:
    event_loop_type: "io_uring" # or "epoll"
    io_threads: 8
```

## Protocol Support

*   **TCP/UDP:** Raw socket handling.
*   **HTTP/1.1 & HTTP/2:** Parsing and session management.
*   **gRPC:** Bidirectional streaming.
*   **WebSocket:** Handshake and framing.
*   **QUIC:** (Experimental) Stub handler.

## License

MIT
