import socket
import sys

def test_udp(port):
    print(f"Testing UDP on port {port}")
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.settimeout(2)
    try:
        msg = b"Hello UDP"
        sock.sendto(msg, ("127.0.0.1", port))
        print("Sent data")
        data, addr = sock.recvfrom(1024)
        print(f"Received: {data}")
        if data == msg:
            print("PASS")
            return True
        else:
            print("FAIL: Mismatch")
            return False
    except Exception as e:
        print(f"FAIL: {e}")
        return False

if __name__ == "__main__":
    if not test_udp(9003):
        sys.exit(1)
