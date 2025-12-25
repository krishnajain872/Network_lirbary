"use client";

import { PageTransition } from "@/components/PageTransition";
import { Badge } from "@/components/ui/badge";
import { Card, CardContent } from "@/components/ui/card";

export default function Protocols() {
  const protocols = [
    { name: "TCP", status: "Stable", desc: "Raw socket stream handling with partial read support." },
    { name: "UDP", status: "Stable", desc: "Datagram support with multicast capabilities." },
    { name: "HTTP/1.1", status: "Stable", desc: "Pipeline support, keep-alive, and chunked transfer encoding." },
    { name: "HTTP/2", status: "Beta", desc: "Multiplexing, header compression (HPACK), and server push." },
    { name: "gRPC", status: "Stable", desc: "High-performance RPC with Protobuf serialization." },
    { name: "WebSocket", status: "Stable", desc: "Full-duplex communication with ping/pong keepalives." },
    { name: "QUIC", status: "Experimental", desc: "UDP-based multiplexed transport (early support)." },
  ];

  return (
    <PageTransition>
      <div className="space-y-8 max-w-4xl">
        <h1 className="text-4xl font-bold mb-4">Supported Protocols</h1>
        <p className="text-xl text-muted-foreground">
           NetworkLib is designed to be protocol-agnostic. The core engine handles bytes; parsers give them meaning.
        </p>

        <div className="grid gap-4">
          {protocols.map((p) => (
             <Card key={p.name} className="flex flex-row items-center p-4">
                <div className="flex-1">
                   <div className="flex items-center gap-2 mb-1">
                      <h3 className="font-bold text-lg">{p.name}</h3>
                      <Badge variant={p.status === "Stable" ? "default" : "secondary"}>{p.status}</Badge>
                   </div>
                   <p className="text-sm text-muted-foreground">{p.desc}</p>
                </div>
             </Card>
          ))}
        </div>

        <section className="space-y-4 pt-6">
           <h2 className="text-2xl font-semibold">Switching Protocols</h2>
           <p className="text-muted-foreground">
              You don't need to rewrite your application logic to switch transport layers.
              The <code>ServiceRegistry</code> allows you to bind the same business logic handler to different protocol adaptors.
           </p>
        </section>
      </div>
    </PageTransition>
  );
}
