# OMS Reference Architecture

The Order Management System (OMS) demonstrates the "Zero-Code Protocol Switching" and high-concurrency capabilities of the library.

## Architecture

| Component | Protocol | Port | Description |
|-----------|----------|------|-------------|
| **Market Data Ingest** | UDP | 9000 | Receives high-frequency price updates. Zero-overhead, loss-tolerant. |
| **Order Entry** | gRPC | 50051 | Bidirectional streaming for order placement and ACKs. Rate-limited. |
| **Market Feed** | WebSocket | 8080 | Pushes real-time order book updates to web clients. |

## Scalability Patterns

### 1. Unified Reactor
All protocols run on the same `epoll`-based Reactor. This avoids context switching between different server processes.

### 2. Lock-Free(ish) OrderBook
The `OrderBook` uses fine-grained locking (std::mutex for Phase 18, simpler than lock-free) but is designed to be replaced with a RingBuffer for 10M+ ops/sec.

### 3. Rate Limiting
The `OrderEntryService` applies a `TokenBucket` to prevent flooding.

## Running the OMS

```bash
# Start Server
./oms_app

# Send Market Data (UDP)
echo "AAPL 150.00" | nc -u localhost 9000

# Connect Web Client
wscat -c ws://localhost:8080
```
