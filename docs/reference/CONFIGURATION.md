# Configuration Reference & Performance Tuning

The Network Library uses a hierarchical YAML configuration system. This document details the available options and provides "Ideal Configurations" for different use cases.

## Configuration Schema

All server configurations are nested under the `server:` root key.

### Network Settings
Control the low-level socket behavior.

```yaml
server:
  network:
    host: "0.0.0.0"       # Bind address
    port: 8080            # Bind port
    protocol: "tcp"       # Primary protocol (tcp, udp)
    socket:
      so_reuseaddr: true  # Allow immediate reuse of port after stop
      so_reuseport: true  # Allow multiple threads to bind same port (scaling)
      tcp_nodelay: true   # Disable Nagle's algorithm (Low Latency)
      tcp_keepalive: true # Detect dead connections
      so_rcvbuf: 2097152  # Kernel receive buffer (2MB)
      so_sndbuf: 2097152  # Kernel send buffer (2MB)
```

### Protocol Support
Enable specific application-layer protocols.

```yaml
server:
  protocols:
    grpc:
      enabled: false
      max_receive_message_size_mb: 100
      streaming:
        bidirectional: true
        max_concurrent_streams: 1000
      compression:
        enabled: true
        default_algorithm: "gzip"
        algorithms: ["gzip", "identity"]
    http:
      enabled: false
      port: 80
      version: "1.1"
    websocket:
      enabled: false
      port: 8080
    tcp:
      enabled: true
      port: 9000
```

### Performance Tuning
Advanced settings to squeeze maximum performance from the hardware.

```yaml
server:
  performance:
    max_connections: 1000000
    io_threads: "auto"        # "auto" or number (e.g., 4)
    event_loop_type: "epoll"  # "epoll" or "io_uring"
    cpu:
      enable_cpu_affinity: true # Pin threads to cores
    memory:
      allocator: "jemalloc"     # "system" or "jemalloc" (if linked)
      connection_pool_size: 10000 # Pre-allocate connection objects
```

### Observability
Configure logging, metrics, and tracing.

```yaml
server:
  observability:
    logging:
      level: "info"       # debug, info, warn, error
      output: "stdout"    # stdout, file
      file_path: "/var/log/server.log"
    metrics:
      enabled: true
      port: 9090
      metrics_list: ["requests_total", "latency_histogram"]
```

---

## Ideal Configurations

### 1. Ultra-Low Latency TCP (HFT / Gaming)

**Goal**: Minimize packet processing delay.
**Key Settings**: `tcp_nodelay`, CPU affinity, minimal logging.

```yaml
server:
  mode: "tcp"
  name: "hft-engine"

  network:
    host: "0.0.0.0"
    port: 7000
    protocol: "tcp"
    socket:
      tcp_nodelay: true        # CRITICAL: Send data immediately
      so_reuseport: false      # Single thread for deterministic latency?
      so_rcvbuf: 8388608       # Large buffers (8MB) to prevent drops during bursts
      so_sndbuf: 8388608

  performance:
    event_loop_type: "epoll"   # Standard, reliable low latency
    io_threads: "1"            # Single thread to avoid context switches
    cpu:
      enable_cpu_affinity: true # Pin to a specific core (e.g., Core 1)
    memory:
      connection_pool_size: 5000
      allocator: "system"      # Or custom pool allocator

  observability:
    logging:
      level: "error"           # CRITICAL: Don't log on hot path
    metrics:
      enabled: false           # Disable overhead
```

### 2. High Throughput gRPC (Microservices)

**Goal**: Maximize requests per second (RPS).
**Key Settings**: `so_reuseport`, multiple threads, compression.

```yaml
server:
  mode: "grpc"
  name: "order-service"

  network:
    socket:
      so_reuseport: true       # CRITICAL: Allow multiple IO threads to accept
      tcp_keepalive: true

  protocols:
    grpc:
      enabled: true
      max_receive_message_size_mb: 20
      streaming:
        max_concurrent_streams: 2000 # High concurrency
      compression:
        enabled: true          # Trade CPU for bandwidth
        default_algorithm: "gzip"

  performance:
    io_threads: "auto"         # Use all available cores
    worker_threads: "auto"     # For business logic
    event_loop_type: "epoll"
    memory:
      connection_pool_size: 50000 # Handle connection spikes
```

### 3. Real-Time WebSocket (Feeds / Chat)

**Goal**: Maintain massive number of idle connections.
**Key Settings**: Memory tuning, Keep-Alives.

```yaml
server:
  mode: "websocket"
  name: "market-data-feed"

  network:
    socket:
      tcp_keepalive: true      # Detect dead clients
      tcp_keepalive_idle_sec: 60
      tcp_nodelay: true        # Fast updates

  protocols:
    websocket:
      enabled: true
      port: 8080

  performance:
    max_connections: 500000    # C100K+ support
    memory:
      max_memory_per_connection_kb: 64 # Limit memory per client
      connection_pool_size: 100000

  observability:
    metrics:
      enabled: true            # Monitor active connection count
```

### 4. High-Performance HTTP API

**Goal**: Fast Request/Response cycle.

```yaml
server:
  mode: "http"

  network:
    socket:
      so_reuseport: true
      tcp_nodelay: false       # Allow packet aggregation for throughput

  protocols:
    http:
      enabled: true
      port: 80
      version: "1.1"

  performance:
    io_threads: "auto"
    enable_zero_copy: true     # Reduce memory copying
```

## Performance Checklist

1.  **OS Tuning**: Ensure `ulimit -n` is high (e.g., 100,000) and sysctl params (`net.core.somaxconn`) are tuned.
2.  **Affinity**: Use `performance.cpu.enable_cpu_affinity` to prevent thread migration.
3.  **Allocators**: If possible, link against `jemalloc` or `tcmalloc` and set `performance.memory.allocator` accordingly.
4.  **Logging**: Never run `info` or `debug` level in production for high-volume services.
