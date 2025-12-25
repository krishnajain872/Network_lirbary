"use client";

import { PageTransition } from "@/components/PageTransition";
import { Card, CardContent } from "@/components/ui/card";
import { CodeBlock } from "@/components/CodeBlock";

export default function DeveloperGuide() {
  return (
    <PageTransition>
      <div className="space-y-8 max-w-4xl">
        <h1 className="text-4xl font-bold mb-4">Developer Guide</h1>
        <p className="text-xl text-muted-foreground">
           Extend NetworkLib and build high-performance applications.
        </p>

        <section className="space-y-4">
           <h2 className="text-2xl font-semibold">C++ API</h2>
           <p className="text-muted-foreground">The core API relies on callbacks and lambdas.</p>

           <CodeBlock
              language="cpp"
              filename="main.cpp"
              code={`#include <networklib/core/server.h>

int main() {
    networklib::Server server("config.yaml");

    server.OnConnect([](auto conn) {
        std::cout << "New connection: " << conn->GetId() << std::endl;
    });

    server.OnMessage([](auto conn, const auto& msg) {
        conn->Send("Echo: " + msg);
    });

    server.Run();
}`}
           />
        </section>

        <section className="space-y-4">
           <h2 className="text-2xl font-semibold">Python Bindings</h2>
           <p className="text-muted-foreground">Use the high-performance core with the ease of Python.</p>

           <CodeBlock
              language="python"
              filename="server.py"
              code={`import networklib

def on_message(conn, msg):
    conn.send(f"Echo: {msg}")

server = networklib.Server("config.yaml")
server.on_message(on_message)
server.run()`}
           />
        </section>

        <section className="space-y-4">
           <h2 className="text-2xl font-semibold">Creating Custom Protocols</h2>
           <p className="text-muted-foreground">
              Implement the <code>ProtocolHandler</code> interface to add support for proprietary protocols.
           </p>
           <ul className="list-disc pl-6 text-muted-foreground">
              <li>Override <code>OnRead</code> to parse raw bytes.</li>
              <li>Override <code>OnWrite</code> to frame outgoing messages.</li>
              <li>Register via <code>server.RegisterProtocol("my-proto", factory)</code>.</li>
           </ul>
        </section>
      </div>
    </PageTransition>
  );
}
