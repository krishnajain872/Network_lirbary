# File Structure Analysis

This document provides a detailed inventory and explanation of the file structure for the Network Library and OMS project.

## Directory Tree

```text
.
├── CMakeLists.txt              # Root build configuration
├── README.md                   # Project overview and entry point
├── build.sh                    # Build helper script
├── core/                       # Core Network Library
│   ├── include/                # Public headers
│   │   └── networklib/         # Main namespace headers
│   │       ├── config/         # Configuration structures
│   │       ├── core/           # Core components (Server, Client, Reactor)
│   │       ├── detail/         # Internal interfaces
│   │       ├── event/          # Event loop and reactor definitions
│   │       ├── logging.h       # Logging interface
│   │       ├── network_lib.h   # Main library entry point
│   │       ├── protocols/      # Protocol handler interfaces
│   │       └── security/       # Security (TLS, DDoS) headers
│   └── src/                    # Library implementation
│       ├── client/             # Client implementation
│       ├── config/             # Config parsing logic
│       ├── event/              # Event loop, Epoll, Reactor impl
│       ├── logging/            # Logger implementation
│       ├── memory/             # Memory management (Pools)
│       ├── protocols/          # Protocol implementations (TCP, etc.)
│       └── security/           # Security implementations
├── oms_client/                 # Reference OMS Client Application
│   ├── config.yaml             # Client configuration
│   ├── main.cpp                # Client entry point
│   └── scenarios/              # Test scenarios (Order placement, etc.)
├── oms_server/                 # Reference OMS Server Application
│   ├── config.yaml             # Server configuration
│   └── main.cpp                # Server entry point (Business Logic)
├── protocols/                  # Protocol Buffers Definitions
│   └── proto/
│       ├── market_data.proto   # Market data messages
│       ├── messages.proto      # System events
│       ├── orders.proto        # Order management messages
│       └── stream_envelope.proto # Core message wrapper
└── tests/                      # Test Suite
    ├── integration/            # Integration tests
    └── unit/                   # Unit tests
```

## Key Files & Directories

### Root Directory
- **`CMakeLists.txt`**: Defines the build process, dependencies (Protobuf, OpenSSL, yaml-cpp), and targets (`network`, `oms_server`, `oms_client`).
- **`build.sh`**: A shell script to automate the CMake configuration and build process.

### Core Library (`core/`)
The `core` directory contains the source code for the reusable network library (`libnetwork.so`).

- **`include/networklib/network_lib.h`**: The primary public API. Users interact with this file to create Servers and Clients.
- **`src/event/event_loop.cpp`**: Implements the event loop using `epoll` or `io_uring`. It handles file descriptor monitoring and task scheduling.
- **`src/event/reactor.cpp`**: Implements the Reactor pattern, dispatching I/O events to registered handlers.
- **`src/protocols/`**: Contains implementations for specific protocols (e.g., `tcp/tcp_handler.cpp`).

### OMS Server (`oms_server/`)
A reference implementation of an Order Management System server.

- **`main.cpp`**: Contains the business logic. It initializes the `Server`, defines the `OrderManager` class, and registers a callback to handle incoming `StreamEnvelope` messages.

### OMS Client (`oms_client/`)
A client application to test and interact with the OMS Server.

- **`scenarios/`**: Contains logic for specific client behaviors, such as `order_placement.cpp` which simulates a trader placing orders.

### Protocols (`protocols/`)
Defines the data interchange format using Protocol Buffers.

- **`stream_envelope.proto`**: This is the most critical file. It defines the `StreamEnvelope` message, which wraps all communication in the system. It includes headers for routing, tracing, security, and the payload itself.

### Tests (`tests/`)
- **`unit/`**: Tests for individual classes (e.g., `EventLoop`, `ConfigParser`).
- **`integration/`**: End-to-end tests ensuring the Client and Server communicate correctly.
