const networklib = require('bindings')('networklib_node');

console.log("Creating EventLoop...");
const loop = new networklib.EventLoop();

console.log("Creating GrpcClient...");
const client = new networklib.GrpcClient(loop);

const host = "127.0.0.1";
const port = 50051;

console.log(`Connecting to ${host}:${port}...`);
if (client.connect(host, port)) {
    console.log("Connected!");
    client.sendUnary("/oms.OrderEntry/Ping", "Hello from Node.js");
} else {
    console.log("Connection failed.");
}

// In a real app, run loop in worker thread or integrate with libuv
// loop.run();
