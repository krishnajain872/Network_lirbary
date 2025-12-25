# Client API Reference

The library provides a unifying Client API for C++ and Python.

## C++ API

### `GrpcClient`

```cpp
#include "networklib/protocols/grpc/grpc_client.h"

auto loop = std::make_shared<EventLoop>();
GrpcClient client(loop);

if (client.Connect("127.0.0.1", 50051)) {
    client.SendUnary("/service/Method", "payload");
}
```

## Python API

```python
import networklib_py

loop = networklib_py.EventLoop()
client = networklib_py.GrpcClient(loop)

client.Connect("127.0.0.1", 50051)
client.SendUnary("/service/Method", "payload")
```

## Node.js API

```javascript
const networklib = require('networklib_binding');
const client = new networklib.GrpcClient(new networklib.EventLoop());
client.connect("127.0.0.1", 50051);
```
