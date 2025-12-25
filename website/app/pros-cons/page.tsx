import { PageTransition } from "@/components/PageTransition";
import { Card, CardContent } from "@/components/ui/card";
import { Check, X } from "lucide-react";

export default function ProsCons() {
  return (
    <PageTransition>
      <div className="space-y-8 max-w-4xl">
        <h1 className="text-4xl font-bold mb-4">Pros & Cons Analysis</h1>
        <p className="text-xl text-muted-foreground">
           An honest assessment of NetworkLib to help you decide if it fits your project.
        </p>

        <div className="grid md:grid-cols-2 gap-8">
           <section className="space-y-4">
              <h2 className="text-2xl font-semibold flex items-center gap-2 text-green-500">
                 <Check className="h-6 w-6" /> Pros
              </h2>
              <Card className="border-green-500/20 bg-green-500/5">
                 <CardContent className="pt-6 space-y-4">
                    <div>
                       <h3 className="font-bold">Extreme Performance</h3>
                       <p className="text-sm text-muted-foreground">Outperforms standard std::asio/Netty by leveraging `io_uring` and thread-per-core design.</p>
                    </div>
                    <div>
                       <h3 className="font-bold">Zero-Code Flexibility</h3>
                       <p className="text-sm text-muted-foreground">Switching from TCP to WebSocket or gRPC is just a config change. No recompilation.</p>
                    </div>
                    <div>
                       <h3 className="font-bold">Predictable Latency</h3>
                       <p className="text-sm text-muted-foreground">Zero-allocation path ensures no GC pauses or malloc spikes.</p>
                    </div>
                 </CardContent>
              </Card>
           </section>

           <section className="space-y-4">
              <h2 className="text-2xl font-semibold flex items-center gap-2 text-red-500">
                 <X className="h-6 w-6" /> Cons
              </h2>
              <Card className="border-red-500/20 bg-red-500/5">
                 <CardContent className="pt-6 space-y-4">
                    <div>
                       <h3 className="font-bold">Complexity</h3>
                       <p className="text-sm text-muted-foreground">C++20 concepts and template metaprogramming make the core codebase harder to contribute to for juniors.</p>
                    </div>
                    <div>
                       <h3 className="font-bold">Linux First</h3>
                       <p className="text-sm text-muted-foreground">Best performance requires Linux 5.1+. Windows/Mac support is second-class (via compatibility layers).</p>
                    </div>
                    <div>
                       <h3 className="font-bold">Smaller Ecosystem</h3>
                       <p className="text-sm text-muted-foreground">Compared to gRPC-Go or Netty, there are fewer third-party plugins and middleware.</p>
                    </div>
                 </CardContent>
              </Card>
           </section>
        </div>

        <section className="space-y-6 pt-8">
            <h2 className="text-2xl font-semibold">When to use NetworkLib?</h2>
            <div className="grid md:grid-cols-2 gap-6">
                <div className="bg-muted p-6 rounded-lg">
                    <h3 className="font-bold mb-2 text-green-400">Ideal For:</h3>
                    <ul className="list-disc pl-5 text-sm space-y-1">
                        <li>High-Frequency Trading Platforms</li>
                        <li>Real-time Ad Bidding Servers</li>
                        <li>Multiplayer Game Backends</li>
                        <li>High-Scale IoT Ingestion</li>
                    </ul>
                </div>
                <div className="bg-muted p-6 rounded-lg">
                    <h3 className="font-bold mb-2 text-orange-400">Consider Alternatives If:</h3>
                    <ul className="list-disc pl-5 text-sm space-y-1">
                        <li>You need a simple CRUD REST API (Use Node/Go).</li>
                        <li>Your team is not comfortable with C++.</li>
                        <li>You need extensive Windows Server support.</li>
                    </ul>
                </div>
            </div>
        </section>
      </div>
    </PageTransition>
  );
}
