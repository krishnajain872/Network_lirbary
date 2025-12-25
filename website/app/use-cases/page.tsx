import { PageTransition } from "@/components/PageTransition";
import { Card, CardContent } from "@/components/ui/card";
import { CodeBlock } from "@/components/CodeBlock";

export default function UseCases() {
  return (
    <PageTransition>
      <div className="space-y-8 max-w-4xl">
        <h1 className="text-4xl font-bold mb-4">Use Cases & Examples</h1>
        <p className="text-xl text-muted-foreground">
           Real-world scenarios where NetworkLib excels.
        </p>

        <section className="space-y-6">
           <h2 className="text-2xl font-semibold">High-Frequency Trading (OMS)</h2>
           <p className="text-muted-foreground">
              Order Management Systems require sub-millisecond latency. NetworkLib's <code>io_uring</code> backend and zero-copy architecture minimize jitter.
           </p>

           <Card>
              <CardContent className="pt-6">
                 <h3 className="font-bold text-lg mb-2">Architecture</h3>
                 <ul className="list-disc pl-5 space-y-1 text-sm text-muted-foreground">
                    <li><strong>Protocol:</strong> Custom Binary Protocol over TCP</li>
                    <li><strong>Thread Model:</strong> Thread Affinity (Core pinning) to reduce cache misses.</li>
                    <li><strong>Optimization:</strong> Zero-allocation object pools for Order objects.</li>
                 </ul>
              </CardContent>
           </Card>

           <CodeBlock
              language="yaml"
              filename="oms_config.yaml"
              code={`server:
  performance:
    event_loop_type: "io_uring"
    thread_affinity: true
    io_threads: 4
    memory:
      enable_huge_pages: true
      enable_memory_pools: true`}
           />
        </section>

        <section className="space-y-6">
           <h2 className="text-2xl font-semibold">Real-Time Game Server</h2>
           <p className="text-muted-foreground">
              MMORPGs need to handle thousands of concurrent players with mixed reliable (TCP) and unreliable (UDP) traffic.
           </p>

           <Card>
              <CardContent className="pt-6">
                 <h3 className="font-bold text-lg mb-2">Solution</h3>
                 <p className="text-sm text-muted-foreground mb-2">
                    Use the <code>ServiceRegistry</code> to handle logic for both TCP and UDP on the same port/loop.
                 </p>
                 <ul className="list-disc pl-5 space-y-1 text-sm text-muted-foreground">
                    <li><strong>TCP:</strong> Chat, Inventory, Transactions.</li>
                    <li><strong>UDP:</strong> Player movement, Physics updates.</li>
                 </ul>
              </CardContent>
           </Card>
        </section>

        <section className="space-y-6">
            <h2 className="text-2xl font-semibold">IoT Gateway</h2>
            <p className="text-muted-foreground">
                Handling 10 million+ idle connections from sensors.
            </p>
            <Card>
                <CardContent className="pt-6">
                    <p className="text-sm text-muted-foreground">
                        NetworkLib's low memory footprint (512 bytes per idle connection) makes it possible to serve millions of devices on a single commodity server.
                    </p>
                </CardContent>
            </Card>
        </section>
      </div>
    </PageTransition>
  );
}
