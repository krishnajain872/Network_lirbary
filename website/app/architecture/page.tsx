import { CodeBlock } from "@/components/CodeBlock";

const architectureDiagram = `
graph TD
    Client[Client] -->|TCP Connect| Acceptor
    Acceptor -->|Round Robin| Worker1[Worker Thread 1]
    Acceptor -->|Round Robin| Worker2[Worker Thread 2]

    subgraph "Worker Thread 1"
        EL1[EventLoop (io_uring)]
        Conn1[Connection]
        Proto1[Protocol Handler]
        EL1 -->|Events| Conn1
        Conn1 -->|Bytes| Proto1
        Proto1 -->|Message| UserCallback
    end
`;

export default function Architecture() {
  return (
    <div className="p-10 space-y-10 max-w-5xl mx-auto">
      <div>
        <h1 className="text-4xl font-bold mb-4">Architecture</h1>
        <p className="text-xl text-slate-400">
          NetworkLib adopts a Reactor Pattern with a One-Loop-Per-Thread architecture to ensure minimal lock contention and maximize CPU cache locality.
        </p>
      </div>

      <section className="space-y-4">
        <h2 className="text-2xl font-bold text-white border-b border-slate-800 pb-2">Project Structure</h2>
        <CodeBlock
          lang="bash"
          code={`network-library/
├── core/                           # Core library implementation
│   ├── include/                    # Public header files
│   └── src/                       # Implementation files
├── protocols/                      # Protocol definitions
│   ├── proto/
│   │   ├── stream_envelope.proto  # Universal envelope
│   │   └── common.proto           # Common types
├── bindings/                       # Language bindings
├── config/                         # Configuration files
└── docker/                         # Docker configurations`}
          filename="File Structure"
        />
      </section>

      <section className="space-y-4">
        <h2 className="text-2xl font-bold text-white border-b border-slate-800 pb-2">StreamEnvelope Protocol</h2>
        <p className="text-slate-400">The universal protocol buffer definition used for all communication.</p>
        <CodeBlock
          lang="protobuf"
          code={`syntax = "proto3";
package networklib;

message StreamEnvelope {
  Header header = 1;
  Payload payload = 2;
  Metadata metadata = 3;
  TraceContext trace = 4;
}

message Header {
  string message_id = 1;
  string correlation_id = 2;
  int64 timestamp = 3;
  Endpoint source = 4;
  Endpoint destination = 5;
  string message_type = 6;
}`}
          filename="protocols/proto/stream_envelope.proto"
        />
      </section>
    </div>
  );
}
