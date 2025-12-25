import sys
import os
import time
import threading
import urllib.request

# Add build dir to path for networklib_py.so
sys.path.append('build')

import networklib_py
import stream_envelope_pb2

def test_server():
    # Create config file
    config_content = """
server:
  mode: http
  network:
    port: 8090
"""
    with open("test_config.yaml", "w") as f:
        f.write(config_content)

    server = networklib_py.NetworkLib.create_server("test_config.yaml")

    def handler(req_bytes):
        # Decode request
        req = stream_envelope_pb2.StreamEnvelope()
        req.ParseFromString(req_bytes)

        print(f"[Python] Received type: {req.header.message_type}")

        # Create response
        resp = stream_envelope_pb2.StreamEnvelope()
        resp.payload.data = b"Hello from Python!"
        return resp.SerializeToString()

    server.register_stream_handler(handler)

    print("Starting server...")
    if not server.start():
        print("Failed to start server")
        sys.exit(1)

    # Wait for start
    time.sleep(1)

    # Client Request using urllib
    try:
        with urllib.request.urlopen("http://localhost:8090/test") as f:
            resp_data = f.read().decode('utf-8')
            print(f"Client received: {resp_data}")
            assert resp_data == "Hello from Python!"
            print("TEST PASSED")
    except Exception as e:
        print(f"TEST FAILED: {e}")
        server.stop()
        sys.exit(1)

    server.stop()
    server.wait()

if __name__ == "__main__":
    test_server()
