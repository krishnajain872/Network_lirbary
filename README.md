# Network Library & OMS System

A high-performance, configuration-driven C++20 network library designed for ultra-low latency trading systems, bundled with a reference Order Management System (OMS).

## Documentation

The detailed technical documentation is located in the `docs/` directory:

*   **[File Structure](docs/reference/FILE_STRUCTURE.md)**: Detailed inventory of the project layout.
*   **[Architecture](docs/architecture/ARCHITECTURE.md)**: High-level and low-level design analysis.
*   **[Component Guide](docs/guides/COMPONENT_GUIDE.md)**: Deep dive into Reactor, EventLoop, and OMS components.
*   **[API Documentation](docs/reference/API_DOCUMENTATION.md)**: Public interfaces and message formats.
*   **[Data Models](docs/reference/DATA_MODELS.md)**: Protobuf definitions for Orders and Messages.
*   **[Design Patterns](docs/architecture/DESIGN_PATTERNS.md)**: Explanation of Reactor, Strategy, Factory patterns used.
*   **[Analysis Report](docs/ANALYSIS_REPORT.md)**: Code quality and architectural assessment.

## Features

*   **Unified API**: Single header (`network_lib.h`) for all networking needs.
*   **Protocol Agnostic**: Switch between TCP, HTTP, gRPC, and WebSocket via configuration.
*   **High Performance**: Built on `epoll` (with `io_uring` support available).
*   **Production Ready**: Includes structured logging, configuration parsing, and resilience patterns.

## Quick Start

### 1. Prerequisites
*   GCC 10+ or Clang 11+ (C++20 support)
*   CMake 3.16+
*   Protobuf, OpenSSL, yaml-cpp

### 2. Build
```bash
./build.sh
```

### 3. Run OMS Server
```bash
./build/oms_server oms_server/config.yaml
```

### 4. Run OMS Client
```bash
./build/oms_client oms_client/config.yaml order_placement
```

## System Overview

The system allows clients to connect to a central server to place orders and receive market data. It uses a **Reactor Pattern** to handle multiple connections efficiently on a single thread.

**Data Flow:**
1.  Client sends `StreamEnvelope` (Protobuf).
2.  Server receives and parses data.
3.  Server routes message to `OrderManager`.
4.  `OrderManager` executes logic and returns response.

See [ARCHITECTURE.md](docs/architecture/ARCHITECTURE.md) for detailed diagrams.
