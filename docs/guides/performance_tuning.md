# Performance Tuning Guide

To achieve 10M+ connections and sub-millisecond latency:

## 1. System Configuration
*   Increase file descriptors: `ulimit -n 1000000`
*   Tune sysctl:
    ```bash
    sysctl -w net.ipv4.tcp_max_syn_backlog=4096
    sysctl -w net.core.somaxconn=4096
    ```

## 2. Library Configuration (`server.yaml`)

### IO_URING
Enable `io_uring` for reduced syscall overhead:
```yaml
performance:
  event_loop_type: "io_uring"
  io_uring_entries: 4096
```

### Memory Pools
Pre-allocate connections to avoid malloc/free churn:
```yaml
performance:
  memory:
    allocator: "jemalloc"
    connection_pool_size: 100000
```

### Thread Pinning
Isolate IO threads to specific cores:
```yaml
performance:
  cpu:
    enable_cpu_affinity: true
    isolated_cores: [0, 1, 2, 3]
```
