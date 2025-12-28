# Network Library & OMS

High-performance C++20 network library and Order Management System (OMS).

## Directory Structure

*   **`api/`**: Protocol Buffer definitions (`proto/`) and generated code (`gen/`).
*   **`apps/`**: Application source code.
    *   `client/`: OMS Client application.
    *   `server/`: OMS Server application.
*   **`lib_logger/`**: Standalone logging library.
    *   `include/logger/`: Public headers.
    *   `src/`: Implementation.
*   **`lib_network/`**: Core network library.
    *   `include/network/`: Public headers.
    *   `src/`: Implementation.
*   **`scripts/`**: Utility scripts.
*   **`docker/`**: Docker build files.

## Build Instructions

### Prerequisites
*   CMake 3.16+
*   GCC 12+ or Clang 15+
*   Protobuf, gRPC, OpenSSL, yaml-cpp

### Building with Script

The `build.sh` script supports building specific targets:

```bash
# Build everything (default)
./build.sh --target=all

# Build specific components
./build.sh --target=client
./build.sh --target=server
./build.sh --target=logger
./build.sh --target=protos
```

Build artifacts are located in:
*   Binaries: `build-<type>/bin/`
*   Libraries: `build-<type>/lib/`

### Manual Build

```bash
mkdir build && cd build
cmake ..
cmake --build .
```
