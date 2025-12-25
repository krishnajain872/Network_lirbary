import { CodeBlock } from "@/components/CodeBlock";

export default function Configuration() {
  return (
    <div className="p-10 space-y-10 max-w-5xl mx-auto">
      <div>
        <h1 className="text-4xl font-bold mb-4">Configuration Guide</h1>
        <p className="text-xl text-slate-400">
          All behavior is controlled by external configuration files. NetworkLib supports hot-reloading and environment-specific overrides.
        </p>
      </div>

      <section className="space-y-4">
        <h2 className="text-2xl font-bold text-white border-b border-slate-800 pb-2">Server Configuration</h2>
        <CodeBlock
          lang="yaml"
          code={`server:
  mode: "grpc"  # http, grpc, rpc, websocket, tcp, udp, mixed

  network:
    host: "0.0.0.0"
    port: 50051
    protocol: "tcp"

  protocols:
    grpc:
      enabled: true
      max_concurrent_streams: 1000
      initial_window_size: 65535
      compression: ["gzip", "deflate"]

  ssl:
    enabled: true
    cert_file: "/certs/server.crt"
    key_file: "/certs/server.key"
    min_tls_version: "1.3"

  performance:
    max_connections: 10000000000  # 10 billion
    io_threads: "auto"
    use_io_uring: true
    enable_zero_copy: true`}
          filename="config/examples/grpc_server.yaml"
        />
      </section>

      <section className="space-y-4">
        <h2 className="text-2xl font-bold text-white border-b border-slate-800 pb-2">Client Configuration</h2>
        <CodeBlock
          lang="yaml"
          code={`client:
  mode: "grpc"

  connection:
    target: "server:50051"
    protocol: "tcp"

  resilience:
    retry:
      enabled: true
      max_attempts: 3
      backoff_multiplier: 2.0
    timeout:
      request_ms: 30000

  load_balancing:
    strategy: "round_robin"
    health_check:
      enabled: true
      interval_ms: 10000`}
          filename="config/examples/grpc_client.yaml"
        />
      </section>
    </div>
  );
}
