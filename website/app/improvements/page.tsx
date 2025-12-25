import { PageTransition } from "@/components/PageTransition";
import { Badge } from "@/components/ui/badge";
import { Card, CardContent } from "@/components/ui/card";

export default function Improvements() {
  return (
    <PageTransition>
      <div className="space-y-8 max-w-4xl">
        <h1 className="text-4xl font-bold mb-4">Improvements & Roadmap</h1>
        <p className="text-xl text-muted-foreground">
           Our vision for the future of NetworkLib.
        </p>

        <section className="space-y-6">
           <h2 className="text-2xl font-semibold">Planned Features</h2>

           <div className="grid gap-4">
              <Card>
                 <CardContent className="p-6 flex items-start gap-4">
                    <Badge>v2.0</Badge>
                    <div>
                       <h3 className="font-bold text-lg">Full QUIC Support</h3>
                       <p className="text-muted-foreground text-sm">
                          Complete implementation of IETF QUIC transport, moving beyond the current experimental stub.
                       </p>
                    </div>
                 </CardContent>
              </Card>

              <Card>
                 <CardContent className="p-6 flex items-start gap-4">
                    <Badge variant="secondary">v2.1</Badge>
                    <div>
                       <h3 className="font-bold text-lg">User-Space Networking (DPDK)</h3>
                       <p className="text-muted-foreground text-sm">
                          Optional backend to bypass the kernel entirely for ultra-low latency (&lt; 5μs).
                       </p>
                    </div>
                 </CardContent>
              </Card>

              <Card>
                 <CardContent className="p-6 flex items-start gap-4">
                    <Badge variant="outline">Future</Badge>
                    <div>
                       <h3 className="font-bold text-lg">Rust Bindings</h3>
                       <p className="text-muted-foreground text-sm">
                          Safe wrappers for Rust integration without FFI overhead.
                       </p>
                    </div>
                 </CardContent>
              </Card>
           </div>
        </section>

        <section className="space-y-6">
           <h2 className="text-2xl font-semibold">Current Limitations</h2>
           <ul className="list-disc pl-6 space-y-2 text-muted-foreground">
              <li>
                 <strong>Windows Support:</strong> Currently reliant on WSL2. Native IOCP support is planned but not prioritized.
              </li>
              <li>
                 <strong>Hot Reload:</strong> Config reload works for protocols, but port changes require a restart.
              </li>
              <li>
                 <strong>HTTP/3:</strong> Dependent on the full QUIC implementation.
              </li>
           </ul>
        </section>

        <section className="space-y-6">
            <h2 className="text-2xl font-semibold">How to Contribute</h2>
            <p className="text-muted-foreground">
                We welcome Pull Requests! Please see our <code>CONTRIBUTING.md</code> on GitHub.
            </p>
        </section>
      </div>
    </PageTransition>
  );
}
