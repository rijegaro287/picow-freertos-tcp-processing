import socket

print("Starting server...")
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

server_socket.bind(("192.168.100.66", 4242))
server_socket.listen()

print("="*50)
print(f"Server listening on {server_socket.getsockname()}")
print("="*50)

while True:
    print("-- Waiting for connection...")

    client_socket, addr = server_socket.accept()
    
    print("="*50)
    print("Connected to", addr)

    data = client_socket.recv(512)
    if not data:
        continue

    print("Received:", data.hex())
    print("="*50)

server_socket.close()
