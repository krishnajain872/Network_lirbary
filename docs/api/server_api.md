# Server API Reference

The Network Library provides a unified `IServer` interface for managing network servers.

## NetworkLib Factory

```cpp
#include "networklib/network_lib.h"

// Create from configuration object
auto server = NetworkLib::CreateServer(config);

// Create from YAML file
auto server = NetworkLib::CreateServer("config.yaml");
```

## IServer Interface

```cpp
class IServer {
public:
    // Lifecycle
    virtual bool Start() = 0;
    virtual void Stop() = 0;
    virtual void Wait() = 0;

    // Generic Stream Handling
    using StreamHandler = std::function<void(const StreamEnvelope& req, StreamEnvelope& resp)>;
    virtual void RegisterStreamHandler(StreamHandler handler) = 0;
};
```

## Usage Example

### Generic Mode (Recommended)

To use the Universal StreamEnvelope architecture:

```cpp
#include "networklib/network_lib.h"
#include "stream_envelope.pb.h" // Generated Protobuf header

int main() {
    auto server = NetworkLib::CreateServer("config.yaml");

    server->RegisterStreamHandler([](const StreamEnvelope& req, StreamEnvelope& resp) {
        // Handle request generically
        std::string type = req.header().message_type();
        std::string body = req.payload().data();

        // Respond
        resp.mutable_payload()->set_data("Response");
    });

    server->Start();
    server->Wait();
}
```

This handler works for **HTTP, gRPC, TCP, and WebSocket** automatically, based on `config.yaml`.

- **HTTP**: `message_type` is Method + Path (e.g., "GET /foo"). Headers are in `metadata`. Body is in `payload`.
- **gRPC**: `message_type` is Service Path. Payload is decoded protobuf message.
- **TCP**: Payload is the serialized `StreamEnvelope` (if configured) or raw bytes.
