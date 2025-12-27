# Design Patterns

This project utilizes several standard software design patterns to ensure modularity, testability, and performance.

## 1. Reactor Pattern
**Location**: `core/src/event/reactor.cpp`

*   **Description**: The architectural pattern used for handling service requests delivered concurrently to a service handler by one or more inputs. The service handler then demultiplexes the incoming requests and dispatches them synchronously to the associated request handlers.
*   **Usage**: The `Reactor` class waits for events on the `EventLoop` (which uses `epoll`). When a socket becomes readable, the Reactor dispatches control to the appropriate `ProtocolHandler`.

## 2. Factory Pattern
**Location**: `networklib/network_lib.h`, `core/src/protocols/protocol_factory.cpp`

*   **Description**: Creates objects without specifying the exact class of object that will be created.
*   **Usage**:
    *   `NetworkLib::CreateServer`: Creates an `IServer` instance.
    *   `ProtocolFactory::Create`: Creates specific protocol handlers (TCP, HTTP) based on configuration strings.

## 3. Observer Pattern
**Location**: `networklib/detail/interfaces.h` (Callbacks)

*   **Description**: Defines a subscription mechanism to notify multiple objects about any events that happen to the object they're observing.
*   **Usage**: The `IServer::RegisterStreamHandler` and `IClient::RegisterMessageHandler` allow the business logic layer to "observe" network events. When a message arrives, the registered function is called.

## 4. Strategy Pattern
**Location**: `core/include/networklib/protocols/protocol_handler.h`

*   **Description**: Defines a family of algorithms, encapsulates each one, and makes them interchangeable.
*   **Usage**: `ProtocolHandler` is the interface. `TcpHandler` is a concrete strategy. The `Connection` class uses a `ProtocolHandler` to process data, allowing the behavior to change based on the protocol (TCP, WebSocket, etc.) without changing the `Connection` class itself.

## 5. Singleton Pattern (Variant)
**Location**: `core/src/logging/logger.cpp` (Implied usage)

*   **Description**: Ensures a class has only one instance and provides a global point of access.
*   **Usage**: While not a strict singleton class, the Logger is initialized globally via `Logger::Initialize` and accessed statically throughout the application via macros like `LOG_INFO`.

## 6. Object Pool Pattern
**Location**: `core/include/networklib/core/memory/object_pool.h`

*   **Description**: Uses a set of initialized objects kept ready to use – a "pool" – rather than allocating and destroying them on demand.
*   **Usage**: Used for managing `Connection` objects or buffers to reduce heap allocation overhead and memory fragmentation, which is critical for low-latency systems.
