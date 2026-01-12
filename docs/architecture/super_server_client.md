# Super-Server and Super-Client Architecture

## Overview
This document describes the architecture for the Universal Reference Server ("Super-Server") and Client ("Super-Client") applications. These applications are designed to demonstrate the full capabilities of the `lib_network` library, supporting multiple protocols simultaneously and providing a comprehensive testing platform.

## Super-Server Architecture
The Super-Server is a multi-protocol server application capable of listening on multiple ports simultaneously, each configured for a specific protocol (TCP, UDP, HTTP, gRPC, WebSocket, QUIC).

### Key Components
1.  **UnifiedServer**: The core class that orchestrates the lifecycle of multiple server instances. It reads the configuration and instantiates the appropriate `Server` or `ProtocolHandler` objects.
2.  **Configuration**: Driven by `config::ServerConfig`, which is enhanced to support per-protocol configurations (enabled status, port, specific settings).
3.  **Protocol Handlers**: Uses the existing `ProtocolFactory` and protocol-specific handlers in `lib_network` to manage connections.
4.  **Logging**: Centralized logging via `lib_logger` with configurable sinks.

### Data Flow
1.  `main` initializes `Logger` and loads `ServerConfig` via `ConfigParser`.
2.  `UnifiedServer` iterates through enabled protocols in `ServerConfig`.
3.  For each enabled protocol, a listener is started on the configured port.
4.  Incoming connections are handled by the respective protocol handlers (e.g., `TcpHandler`, `HttpHandler`).

## Super-Client Architecture
The Super-Client is a scenario-driven testing tool designed to validate the server's functionality across all supported protocols.

### Key Components
1.  **UnifiedClient**: The main class responsible for executing test scenarios.
2.  **Scenario Engine**: Parses a YAML-based scenario file defining a sequence of operations (Connect, Send, Expect, Disconnect).
3.  **Client Adapters**: Protocol-specific client implementations (or usage of `lib_network` client classes) to perform the actual network I/O.

### Scenario Structure
Scenarios are defined in YAML:
```yaml
scenarios:
  - name: "TCP Echo"
    protocol: tcp
    target: "127.0.0.1:8080"
    steps:
      - action: connect
      - action: send
        data: "Hello"
      - action: expect
        data: "Hello"
      - action: disconnect
```

## Build & Dependencies
Both applications are built using CMake and link against `lib_network`, `lib_logger`, and `protos`.
