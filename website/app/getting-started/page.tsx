"use client";

import { PageTransition } from "@/components/PageTransition";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Badge } from "@/components/ui/badge";
import { CodeBlock } from "@/components/CodeBlock";

export default function GettingStarted() {
  return (
    <PageTransition>
      <div className="space-y-8 max-w-4xl">
        <div>
           <h1 className="text-4xl font-bold mb-4">Getting Started</h1>
           <p className="text-xl text-muted-foreground">
              Everything you need to install and run NetworkLib in minutes.
           </p>
        </div>

        <section className="space-y-4">
           <h2 className="text-2xl font-semibold">Prerequisites</h2>
           <Card>
              <CardContent className="pt-6">
                 <ul className="list-disc pl-6 space-y-2 text-muted-foreground">
                    <li><strong className="text-foreground">OS:</strong> Linux (Kernel 5.1+ recommended for io_uring)</li>
                    <li><strong className="text-foreground">Compiler:</strong> GCC 11+ or Clang 14+ (C++20 support required)</li>
                    <li><strong className="text-foreground">Build System:</strong> CMake 3.20+ and Ninja</li>
                    <li><strong className="text-foreground">Dependencies:</strong> OpenSSL, Protobuf, liburing, libyaml-cpp, gtest</li>
                 </ul>
              </CardContent>
           </Card>
        </section>

        <section className="space-y-4">
           <h2 className="text-2xl font-semibold">Installation Methods</h2>

           <h3 className="text-xl font-medium mt-6">1. Docker (Recommended)</h3>
           <p className="text-muted-foreground">The fastest way to get up and running is using the provided Docker environment.</p>

           <CodeBlock
              language="bash"
              filename="Terminal"
              code={`git clone https://github.com/your-repo/network-library.git
cd network-library/docker
docker compose up --build -d`}
           />

           <h3 className="text-xl font-medium mt-6">2. Manual Build</h3>
           <p className="text-muted-foreground">For development on your local machine.</p>

           <CodeBlock
              language="bash"
              filename="Terminal"
              code={`# Install dependencies (Ubuntu/Debian)
sudo apt install build-essential cmake ninja-build libssl-dev libprotobuf-dev protobuf-compiler liburing-dev libyaml-cpp-dev libgtest-dev

# Clone and Build
mkdir build && cd build
cmake -G Ninja -DENABLE_TESTS=ON ..
cmake --build .`}
           />
        </section>

        <section className="space-y-4">
           <h2 className="text-2xl font-semibold">Running Your First Server</h2>
           <p className="text-muted-foreground">NetworkLib comes with example configurations. To start a basic TCP echo server:</p>

           <CodeBlock
              language="bash"
              filename="Terminal"
              code={`./bin/network_server --config ../config/examples/tcp/echo_server.yaml`}
           />

           <p className="mt-4">You should see:</p>
           <Card className="bg-black border-zinc-800">
              <CardContent className="p-4 font-mono text-sm text-green-400">
                 [INFO] Server started on 0.0.0.0:8080 (TCP)<br/>
                 [INFO] EventLoop: io_uring initialized with 2048 entries<br/>
                 [INFO] Workers: 8 threads active
              </CardContent>
           </Card>
        </section>
      </div>
    </PageTransition>
  );
}
