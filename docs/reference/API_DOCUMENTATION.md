# API Documentation

## Public C++ API

The primary entry point is `networklib::NetworkLib`.

### NetworkLib Factory

Header: `networklib/network_lib.h`

```cpp
class NetworkLib {
public:
    // Create a server instance based on the YAML config
    static std::shared_ptr<IServer> CreateServer(const std::string& config_path);

    // Create a client instance based on the YAML config
    static std::shared_ptr<IClient> CreateClient(const std::string& config_path);
};
```

### IServer Interface

Header: `networklib/detail/interfaces.h`

```cpp
class IServer {
public:
    // Start the server (non-blocking, starts internal threads)
    virtual bool Start() = 0;

    // Stop the server and close connections
    virtual void Stop() = 0;

    // Block the current thread until the server stops
    virtual void Wait() = 0;

    // Register the callback for processing business messages
    virtual void RegisterStreamHandler(StreamHandler handler) = 0;
};
```

**StreamHandler Signature**:
```cpp
using StreamHandler = std::function<void(
    const StreamEnvelope& req,       // Incoming request
    StreamEnvelope& resp,            // Response object to populate
    std::shared_ptr<IStreamContext> ctx // Context to write async responses
)>;
```

### IClient Interface

Header: `networklib/detail/interfaces.h`

```cpp
class IClient {
public:
    // Connect to the server defined in config
    virtual bool Connect() = 0;

    // Close connection
    virtual void Disconnect() = 0;

    // Send raw string data (low-level)
    virtual bool Send(const std::string& data) = 0;

    // Send a structured StreamEnvelope (recommended)
    virtual bool Send(const StreamEnvelope& envelope) = 0;

    // Register callback for incoming messages
    virtual void RegisterMessageHandler(MessageHandler handler) = 0;
};
```

## Protocol API (StreamEnvelope)

All communication uses the `StreamEnvelope` protobuf message.

### Request Format

| Field | Description |
|-------|-------------|
| `header.message_type` | The command to execute (e.g., "ORDER_NEW", "SUBSCRIBE"). |
| `header.correlation_id` | Unique ID to match response to request. |
| `payload.data` | Serialized specific proto message (e.g., `Order`). |

### Response Format

| Field | Description |
|-------|-------------|
| `header.message_type` | Usually matches request or indicates error. |
| `header.correlation_id` | Matches the request's ID. |
| `payload.data` | Serialized response proto (e.g., `OrderResponse`). |

### Common Message Types

*   `ORDER_NEW`: Place a new order. Payload: `Order`.
*   `ORDER_CANCEL`: Cancel an order. Payload: `OrderCancel`.
*   `MARKET_DATA_UPDATE`: Market data broadcast. Payload: `MarketData`.
