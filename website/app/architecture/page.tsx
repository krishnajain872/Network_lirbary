import { PageTransition } from "@/components/PageTransition";
import { Card, CardContent } from "@/components/ui/card";
import { Badge } from "@/components/ui/badge";
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

const classDiagram = `
classDiagram
    class Server {
        +Run()
        +OnMessage()
    }
    class EventLoop {
        +Run()
        +AddFd()
        -poller_ : Poller
    }
    class Poller {
        <<interface>>
        +Poll()
    }
    class EpollPoller {
        +Poll()
    }
    class IoUringPoller {
        +Poll()
    }
    class Connection {
        +HandleRead()
        +HandleWrite()
        +Send()
        -input_buffer_ : Buffer
    }

    Server --> EventLoop
    EventLoop --> Poller
    Poller <|-- EpollPoller
    Poller <|-- IoUringPoller
    EventLoop --> Connection
`;

export default function Architecture() {
  return (
    <PageTransition>
      <div className="space-y-12 max-w-4xl">
        <div>
          <h1 className="text-4xl font-bold mb-4">System Architecture</h1>
          <p className="text-xl text-muted-foreground">
             A deep dive into the High-Level Design (HLD) and Low-Level Design (LLD) of NetworkLib.
          </p>
        </div>

        {/* HLD SECTION */}
        <section className="space-y-6">
           <h2 className="text-3xl font-bold tracking-tight border-b pb-2">High-Level Design (HLD)</h2>

           <h3 className="text-xl font-semibold mt-6">Reactor Pattern</h3>
           <Card>
              <CardContent className="pt-6">
                 <p className="text-muted-foreground mb-4">
                    NetworkLib adopts a <strong>Reactor Pattern</strong> with a <strong className="text-primary">One-Loop-Per-Thread</strong> architecture.
                    This ensures minimal lock contention and maximizes CPU cache locality. Each thread has its own `EventLoop` and is pinned to a specific CPU core.
                 </p>
                 <div className="flex justify-center p-4 bg-muted/20 rounded-lg">
                    <Mermaid chart={architectureDiagram} />
                 </div>
              </CardContent>
           </Card>

           <div className="grid md:grid-cols-2 gap-6 mt-4">
              <Card>
                 <CardContent className="pt-6">
                    <h3 className="font-bold text-lg mb-2">Non-Blocking I/O</h3>
                    <p className="text-sm text-muted-foreground">
                       All networking operations are non-blocking. The system never waits for `read()` or `write()`.
                       Instead, it registers interest in events via `epoll` or `io_uring` and reacts via callbacks.
                    </p>
                 </CardContent>
              </Card>
              <Card>
                 <CardContent className="pt-6">
                    <h3 className="font-bold text-lg mb-2">Zero-Copy Data Path</h3>
                    <p className="text-sm text-muted-foreground">
                       Data flows from the kernel to the user application with minimal copying.
                       We utilize `splice()` for proxying and `sendfile()` for static assets.
                    </p>
                 </CardContent>
              </Card>
           </div>
        </section>

        {/* LLD SECTION */}
        <section className="space-y-6">
           <h2 className="text-3xl font-bold tracking-tight border-b pb-2">Low-Level Design (LLD)</h2>

           <h3 className="text-xl font-semibold mt-6">Core Class Hierarchy</h3>
           <p className="text-muted-foreground">
              The core library is built around strict segregation of duties.
           </p>
           <div className="bg-card border rounded-lg p-4 overflow-hidden">
              <Mermaid chart={classDiagram} />
           </div>

           <div className="space-y-6 mt-6">
              <Card>
                 <CardContent className="pt-6">
                    <h4 className="font-bold text-lg flex items-center gap-2">
                       <Badge variant="outline">Core</Badge> EventLoop & Poller
                    </h4>
                    <p className="text-sm text-muted-foreground mt-2">
                       The `EventLoop` class is the heart of the system. It delegates actual syscalls to a `Poller` strategy:
                    </p>
                    <ul className="list-disc pl-5 mt-2 space-y-1 text-sm text-muted-foreground">
                       <li><strong>EpollPoller:</strong> Uses `epoll_wait` (Linux legacy). Level-triggered by default.</li>
                       <li><strong>IoUringPoller:</strong> Uses the modern `io_uring` submission/completion queues for async batching.</li>
                    </ul>
                 </CardContent>
              </Card>

              <Card>
                 <CardContent className="pt-6">
                    <h4 className="font-bold text-lg flex items-center gap-2">
                       <Badge variant="outline">Core</Badge> Connection State Machine
                    </h4>
                    <p className="text-sm text-muted-foreground mt-2">
                       Each `Connection` object manages a file descriptor and buffers. It transitions through:
                    </p>
                    <div className="flex gap-2 mt-3 flex-wrap">
                       <Badge variant="secondary">kConnecting</Badge>
                       <Badge variant="secondary">kHandshaking (TLS)</Badge>
                       <Badge variant="secondary">kConnected</Badge>
                       <Badge variant="secondary">kDisconnecting</Badge>
                       <Badge variant="secondary">kDisconnected</Badge>
                    </div>
                    <p className="text-sm text-muted-foreground mt-3">
                       <strong>Write Handling:</strong> The `HandleWrite()` method manages a userspace output buffer.
                       It only enables `EPOLLOUT` when the kernel socket buffer is full (`EAGAIN`), preventing busy loops.
                    </p>
                 </CardContent>
              </Card>

              <Card>
                 <CardContent className="pt-6">
                    <h4 className="font-bold text-lg flex items-center gap-2">
                       <Badge variant="outline">Memory</Badge> Object Pools
                    </h4>
                    <p className="text-sm text-muted-foreground mt-2">
                       To avoid `malloc/free` latency during runtime, we use `ObjectPool&lt;T&gt;` for:
                    </p>
                    <ul className="list-disc pl-5 mt-2 space-y-1 text-sm text-muted-foreground">
                       <li>`Connection` objects (reused immediately after disconnect).</li>
                       <li>`Buffer` chunks (4KB blocks).</li>
                    </ul>
                    <p className="text-sm text-muted-foreground mt-2">
                       The pool uses a `std::vector` stack with a mutex, but contention is low because each thread has its own local pools (Thread-Local Storage pattern).
                    </p>
                 </CardContent>
              </Card>
           </div>
        </section>
      </div>
    </PageTransition>
  );
}
