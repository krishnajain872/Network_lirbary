# Data Models

The system uses **Protocol Buffers (protobuf)** for all data serialization. This ensures high performance, language neutrality, and backward compatibility.

## Core Message Structure

All messages in the system are wrapped in a unified `StreamEnvelope` message. This provides a consistent way to handle routing, metadata, tracing, and security across all protocols (TCP, WebSocket, etc.).

### StreamEnvelope (`protocols/proto/stream_envelope.proto`)

The `StreamEnvelope` is the top-level container.

```protobuf
message StreamEnvelope {
  Header header = 1;          // Routing and control info
  Payload payload = 2;        // The actual business data
  Metadata metadata = 3;      // Extensible key-value pairs
  TraceContext trace = 4;     // Distributed tracing (W3C style)
  SecurityContext security = 5; // Auth tokens, scopes
  QoSSettings qos = 6;        // Reliability and rate limits
}
```

#### Key Sub-Components

*   **Header**: Contains `message_id`, `correlation_id` (for request-response), `source` and `destination` endpoints, and `message_type` (e.g., "ORDER_NEW").
*   **Payload**: Contains the actual serialized data (bytes) and format information (`content_type`, `encoding`, `compression`).
*   **TraceContext**: `trace_id` and `span_id` for observability across microservices.

## Business Domain Models

The business logic for the Order Management System (OMS) is defined in specific proto files.

### Order Management (`protocols/proto/orders.proto`)

Defines the structure for placing and managing orders.

**Order**
```protobuf
message Order {
  string order_id = 1;
  string symbol = 2;          // e.g., "AAPL"
  OrderType type = 3;         // LIMIT, MARKET, STOP_LOSS, etc.
  Side side = 4;              // BUY, SELL
  double price = 5;
  int64 quantity = 6;
  int64 timestamp = 7;
  string client_order_id = 8; // ID assigned by the client
  TimeInForce time_in_force = 9;
  string account_id = 10;
}
```

**OrderResponse**
```protobuf
message OrderResponse {
  string order_id = 1;
  string client_order_id = 2;
  OrderStatus status = 3;     // NEW, FILLED, REJECTED, etc.
  string message = 4;         // Human-readable status message
  double executed_price = 5;
  int64 executed_quantity = 6;
  int64 remaining_quantity = 7;
}
```

### Market Data (`protocols/proto/market_data.proto`)

Defines market data updates.

**MarketData**
```protobuf
message MarketData {
  string symbol = 1;
  double last_price = 2;
  double last_quantity = 3;
  double bid_price = 4;
  double bid_quantity = 5;
  double ask_price = 6;
  double ask_quantity = 7;
  int64 timestamp_us = 8;
}
```

### System Events (`protocols/proto/messages.proto`)

Used for internal system notifications.

**Event**
```protobuf
message Event {
    EventType type = 1;       // SYSTEM_CONNECT, SYSTEM_ERROR, etc.
    string connection_id = 2;
    bytes data = 3;
    int64 timestamp_us = 4;
    int32 error_code = 5;
}
```

## Data Flow

1.  **Client** creates a specific message (e.g., `Order`).
2.  Client serializes the `Order` to bytes.
3.  Client creates a `StreamEnvelope`.
    *   Sets `header.message_type = "ORDER_NEW"`.
    *   Sets `payload.data = serialized_order`.
4.  **Network Layer** serializes the `StreamEnvelope` and sends it over the wire.
5.  **Server** receives and deserializes `StreamEnvelope`.
6.  Server examines `header.message_type`.
7.  Server deserializes `payload.data` back into an `Order` object.
