# Client API Reference

## IClient Interface

The `IClient` interface provides a generic way to connect and send messages.

```cpp
#include "networklib/network_lib.h"

// Create from file
auto client = NetworkLib::CreateClient("client.yaml");

if (client->Connect()) {
    // Send generic raw data
    client->Send("Raw Data");

    // Send StreamEnvelope (requires header)
    #include "stream_envelope.pb.h"
    StreamEnvelope env;
    env.mutable_payload()->set_data("Payload");
    client->Send(env);

    client->Disconnect();
}
```

## Configuration

`client.yaml`:

```yaml
client:
  mode: "http" # or "tcp", "grpc"
  connection:
    host: "127.0.0.1"
    port: 8080
```
