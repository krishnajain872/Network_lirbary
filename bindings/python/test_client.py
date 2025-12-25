import sys
import os

# Add build dir to path if needed, or assume installed
# sys.path.append('../../build') 

try:
    import networklib_py
except ImportError:
    print("Error: networklib_py module not found. Make sure it's built and in PYTHONPATH.")
    sys.exit(1)

def main():
    print("Creating EventLoop...")
    loop = networklib_py.EventLoop()
    
    print("Creating Client...")
    client = networklib_py.GrpcClient(loop)
    
    host = "127.0.0.1"
    port = 50051
    
    print(f"Connecting to {host}:{port}...")
    if client.Connect(host, port):
        print("Connected!")
        # Send a test message (raw string for now, proto serialization in python would happen here)
        client.SendUnary("/oms.OrderEntry/Ping", "Hello from Python")
    else:
        print("Connection failed.")

if __name__ == "__main__":
    main()
