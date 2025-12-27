# Component Guide

This guide provides detailed information on the major components of the system.

## Core Components

### 1. Reactor (`core/src/event/reactor.cpp`)

The `Reactor` is the central nervous system of the server.

*   **Responsibilities**:
    *   Manages the lifecycle of the `EventLoop`.
    *   Maintains the registry of active `Connection`s.
    *   Handles the `Accept` event for new incoming connections.
    *   Integrates with `DdosProtection` and `TlsContext`.

**Key Methods**:
*   `RegisterServer(port, handler)`: Sets up a listening socket.
*   `Run()`: Starts the blocking event loop.

### 2. EventLoop (`core/src/event/event_loop.cpp`)

The execution engine.

*   **Design**: Single-threaded loop.
*   **Concurrency**: Supports cross-thread task scheduling via `QueueInLoop`.
*   **Poller Abstraction**: Hides the details of `epoll` vs `io_uring`.

**Code Example (Task Scheduling):**
```cpp
// Schedule a task from another thread
loop->QueueInLoop([connection, data]() {
    connection->Send(data);
});
```

### 3. Connection (`core/src/connection.cpp`)

Represents a single client connection.

*   **State**: Buffers for input and output.
*   **Ownership**: Shared pointer managed by the `Reactor`.
*   **Interface**: `Read()`, `Write()`, `Close()`.

### 4. Configuration (`core/src/config/`)

The system is configuration-driven using YAML.

*   **Features**:
    *   Hierarchical parsing.
    *   Hot Reload support (`ConfigHotReload`).
    *   Environment variable substitution (likely supported by `yaml-cpp` usage).

## OMS Components

### 1. Order Manager (`oms_server/main.cpp`)

The core business logic unit.

*   **Concurrency**: Uses `std::shared_mutex` for Read-Write locking. Allows multiple readers (status checks) but exclusive writer (order placement).
*   **Storage**: In-memory `std::map<string, Order>`.
*   **Logic**: Validates order parameters (price, quantity) and assigns IDs.

**Key Methods**:
*   `PlaceOrder(Order)`: Validates and stores.
*   `CancelOrder(id)`: Removes/Updates status.

### 2. Market Data Publisher (`oms_server/main.cpp`)

Simulates a market data feed.

*   **Design**: Runs on a separate thread.
*   **Action**: Periodically generates mock updates (Ticks).
*   **Integration**: Would use the `Server` to broadcast messages to subscribed clients.

## Protocols

### 1. ProtocolHandler

The base class for all protocol parsers.

*   **Design Pattern**: Strategy Pattern. The `Connection` uses a `ProtocolHandler` strategy to interpret data.
*   **Extension**: To add HTTP support, one would implement `HttpHandler : public ProtocolHandler`.
