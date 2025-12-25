"use client";

import { PageTransition } from "@/components/PageTransition";
import { Card, CardContent } from "@/components/ui/card";
import { CodeBlock } from "@/components/CodeBlock";

export default function Configuration() {
  return (
    <PageTransition>
      <div className="space-y-8 max-w-4xl">
        <h1 className="text-4xl font-bold mb-4">Configuration Guide</h1>
        <p className="text-xl text-muted-foreground">
           Control every aspect of the server via YAML. No code changes required.
        </p>

        <section className="space-y-4">
           <h2 className="text-2xl font-semibold">Server Config</h2>
           <p className="text-muted-foreground">
              The root configuration block `server` defines the environment and basic identity.
           </p>
           <CodeBlock
              language="yaml"
              filename="config.yaml"
              code={`server:
  mode: "tcp"           # tcp, udp, http, grpc, websocket
  environment: "prod"   # dev, prod
  network:
    host: "0.0.0.0"
    port: 8080
    socket:
      so_reuseaddr: true
      tcp_nodelay: true`}
           />
        </section>

        <section className="space-y-4">
           <h2 className="text-2xl font-semibold">Performance Tuning</h2>
           <p className="text-muted-foreground">
              Critical settings for optimizing throughput and latency.
           </p>
           <CodeBlock
              language="yaml"
              filename="config.yaml"
              code={`server:
  performance:
    io_threads: "auto"    # or number, e.g., 8
    event_loop_type: "io_uring" # io_uring (Linux 5.1+) or epoll
    enable_zero_copy: true
    memory:
      enable_huge_pages: true
      connection_pool_size: 50000`}
           />
           <Card>
              <CardContent className="pt-6 text-sm text-muted-foreground">
                 <strong>Note:</strong> `io_uring` requires `memlock` limits to be increased on the host system.
              </CardContent>
           </Card>
        </section>

        <section className="space-y-4">
           <h2 className="text-2xl font-semibold">Protocol Specifics</h2>

           <h3 className="text-lg font-medium">HTTP/1.1 & HTTP/2</h3>
           <CodeBlock
              language="yaml"
              filename="config.yaml"
              code={`server:
  protocols:
    http:
      enabled: true
      version: "1.1"
      keepalive_timeout_ms: 60000`}
           />

           <h3 className="text-lg font-medium mt-4">gRPC</h3>
           <CodeBlock
              language="yaml"
              filename="config.yaml"
              code={`server:
  protocols:
    grpc:
      enabled: true
      streaming:
        bidirectional: true
        max_concurrent_streams: 1000
      compression:
        enabled: true
        default_algorithm: "gzip"`}
           />
        </section>
      </div>
    </PageTransition>
  );
}
