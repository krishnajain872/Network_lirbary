# Network Library & OMS System

A high-performance, configuration-driven C++20 network library designed for ultra-low latency trading systems, bundled with a reference Order Management System (OMS).

## Documentation

*   **[Usage Guide (README_OMS.md)](README_OMS.md):** Instructions for building, running, and testing the OMS Server and Client.
*   **[Technical Architecture (TECHNICAL_ARCHITECTURE.md)](TECHNICAL_ARCHITECTURE.md):** Detailed breakdown of High-Level Design (HLD), Low-Level Design (LLD), component analysis, and data flow.

## Features

*   **Unified API:** Single header (`network_lib.h`) for all networking needs.
*   **Protocol Agnostic:** Switch between TCP, HTTP, gRPC, and WebSocket via configuration.
*   **High Performance:** Built on `epoll` (with `io_uring` support available).
*   **Production Ready:** Includes structured logging, configuration parsing, and resilience patterns.

## Quick Start

### Build
```bash
./build.sh
```

### Run OMS Server
```bash
./build/oms_server oms_server/config.yaml
```

### Run OMS Client
```bash
./build/oms_client oms_client/config.yaml order_placement
```

## License

MIT
