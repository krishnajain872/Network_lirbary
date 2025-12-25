# Building Network Library

This project uses CMake. It supports building the core library as either a Shared Library (Dynamic, default) or a Static Library.

## Prerequisites

Ensure you have the following installed:
- CMake (3.16+)
- GCC/G++ (12+) or Clang (15+)
- Ninja (optional, recommended)
- Protobuf
- OpenSSL
- yaml-cpp
- liburing
- Google Test
- Python 3 (for bindings)

## Building

### Dynamic Library (Default)

To build the library as a shared object (`.so`):

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

This will produce `libnetworklib.so` in the build directory.

### Static Library

To build the library as a static archive (`.a`), set `BUILD_SHARED_LIBS=OFF`:

```bash
mkdir build
cd build
cmake -DBUILD_SHARED_LIBS=OFF ..
cmake --build .
```

This will produce `libnetworklib.a` in the build directory.

## Running Tests

After building, you can run the tests using `ctest`:

```bash
cd build
ctest --output-on-failure
```

## Docker

To build the Docker image (which uses the default dynamic build):

```bash
docker build -f docker/Dockerfile.build .
```
