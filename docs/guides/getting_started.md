# Getting Started

## Installation

```bash
mkdir build && cd build
cmake ..
cmake --build .
sudo cmake --install .
```

## Quick Start (C++)

1. Create a configuration file `server.yaml`:

```yaml
server:
  mode: http
  network:
    port: 8080
```

2. Write the server code:

```cpp
#include "networklib/network_lib.h"
#include "stream_envelope.pb.h"

int main() {
    auto server = networklib::NetworkLib::CreateServer("server.yaml");

    server->RegisterStreamHandler([](const auto& req, auto& resp) {
        resp.mutable_payload()->set_data("Hello World");
    });

    server->Start();
    server->Wait();
}
```

3. Run it!
To switch to **gRPC**, just change `mode: grpc` in `server.yaml`. Zero code changes required.

## Python

```python
import networklib_py
import stream_envelope_pb2

server = networklib_py.NetworkLib.create_server("server.yaml")

def handler(req_bytes):
    resp = stream_envelope_pb2.StreamEnvelope()
    resp.payload.data = b"Hello from Python"
    return resp.SerializeToString()

server.register_stream_handler(handler)
server.start()
server.wait()
```
