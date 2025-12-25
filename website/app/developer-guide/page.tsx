import { CodeBlock } from "@/components/CodeBlock";

export default function DeveloperGuide() {
  return (
    <div className="p-10 space-y-10 max-w-5xl mx-auto">
      <div>
        <h1 className="text-4xl font-bold mb-4">Developer Guide</h1>
        <p className="text-xl text-slate-400">
          Complete API reference for building high-performance applications.
        </p>
      </div>

      <section className="space-y-4">
        <h2 className="text-2xl font-bold text-white border-b border-slate-800 pb-2">C++ API Usage</h2>
        <CodeBlock
          lang="cpp"
          code={`#include "network_lib.h"

int main() {
    // Load configuration
    Config config = Config::LoadFromFile("config/grpc_server.yaml");

    // Create server
    auto server = NetworkLib::CreateServer(config);

    // Register stream handler
    server->RegisterStreamHandler([](
        const StreamEnvelope& request,
        StreamEnvelope& response,
        StreamContext& ctx
    ) {
        response.mutable_payload()->set_data(
            ProcessData(request.payload().data())
        );
        return Status::OK;
    });

    server->Start();
    return 0;
}`}
          filename="main.cpp"
        />
      </section>

      <section className="space-y-4">
        <h2 className="text-2xl font-bold text-white border-b border-slate-800 pb-2">VSCode Debugging</h2>
        <p className="text-slate-400">Debugging is supported via Docker and Remote Containers.</p>
        <CodeBlock
          lang="json"
          code={`{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "Attach to Docker - Server",
      "type": "cppdbg",
      "request": "attach",
      "program": "/usr/local/bin/network-server",
      "miDebuggerServerAddress": "localhost:2345",
      "sourceFileMap": { "/build": "\${workspaceFolder}" }
    }
  ]
}`}
          filename=".vscode/launch.json"
        />
      </section>
    </div>
  );
}
