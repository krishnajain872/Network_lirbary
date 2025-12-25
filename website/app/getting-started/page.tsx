export default function GettingStarted() {
  return (
    <div className="p-10 space-y-6">
      <h1 className="text-4xl font-bold">Getting Started</h1>
      <p className="text-xl text-muted-foreground">Everything you need to install and run NetworkLib.</p>

      <h2 className="text-2xl font-bold">Installation</h2>
      <pre className="bg-muted p-4 rounded-lg overflow-x-auto">
        <code>
git clone https://github.com/your-repo/network-library.git
cd network-library/docker
docker compose up --build -d
        </code>
      </pre>
    </div>
  );
}
