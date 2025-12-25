"use client";

import { PageTransition } from "@/components/PageTransition";
import { Card, CardContent } from "@/components/ui/card";
import { Mermaid } from "@/components/Mermaid";

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
    <PageTransition>
      <div className="space-y-8 max-w-4xl">
        <h1 className="text-4xl font-bold mb-4">System Architecture</h1>
        <p className="text-xl text-muted-foreground">
           Deep dive into the reactor pattern, threading model, and memory management.
        </p>

        <section className="space-y-6">
           <h2 className="text-2xl font-semibold">High-Level Design (HLD)</h2>
           <p className="text-muted-foreground">
              NetworkLib adopts a <strong>Reactor Pattern</strong> with a <strong className="text-primary">One-Loop-Per-Thread</strong> architecture.
              This ensures minimal lock contention and maximizes CPU cache locality.
           </p>

           <Card className="p-6 overflow-hidden">
             <Mermaid chart={architectureDiagram} />
           </Card>

           <div className="grid md:grid-cols-2 gap-6">
              <Card>
                 <CardContent className="pt-6">
                    <h3 className="font-bold text-lg mb-2">Core Components</h3>
                    <ul className="list-disc pl-5 space-y-1 text-sm text-muted-foreground">
                       <li><strong>EventLoop:</strong> Wraps `epoll` or `io_uring` to handle non-blocking I/O events.</li>
                       <li><strong>Acceptor:</strong> Handles incoming connections and distributes them to workers.</li>
                       <li><strong>Worker Threads:</strong> Execute application logic and protocol parsing independently.</li>
                       <li><strong>Connection:</strong> Represents a client session, managing buffers and state.</li>
                    </ul>
                 </CardContent>
              </Card>
              <Card>
                 <CardContent className="pt-6">
                    <h3 className="font-bold text-lg mb-2">Data Flow</h3>
                    <ol className="list-decimal pl-5 space-y-1 text-sm text-muted-foreground">
                       <li>Client initiates connection (TCP handshake).</li>
                       <li>Main Acceptor accepts socket and hands it off to a Worker via Round-Robin.</li>
                       <li>Worker registers socket with its local `EventLoop`.</li>
                       <li>Data arrives -&gt; triggers Read Event -&gt; Protocol Decoder -&gt; User Callback.</li>
                    </ol>
                 </CardContent>
              </Card>
           </div>
        </section>

        <section className="space-y-6">
           <h2 className="text-2xl font-semibold">Low-Level Details (LLD)</h2>

           <h3 className="text-xl font-medium">Memory Management</h3>
           <p className="text-muted-foreground">
              To achieve zero-allocation during steady state, we use:
           </p>
           <ul className="list-disc pl-6 text-muted-foreground">
              <li><strong>Object Pools:</strong> Recyclable `Connection` and `Buffer` objects.</li>
              <li><strong>Ring Buffers:</strong> For lock-free communication between threads (if needed).</li>
              <li><strong>Zero-Copy:</strong> Uses `splice` and `sendfile` where applicable.</li>
           </ul>

           <h3 className="text-xl font-medium mt-6">Protocol Abstraction</h3>
           <p className="text-muted-foreground">
              Protocols are implemented as state machines that consume a `ByteStream`.
              Switching protocols is as simple as swapping the `ProtocolHandler` implementation on the connection.
           </p>
        </section>
      </div>
    </PageTransition>
  );
}
