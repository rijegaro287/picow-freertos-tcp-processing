import socket

sample_width = 2
buffer_size = 128

print("Starting server...")
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

server_socket.bind(("192.168.100.66", 4242))
server_socket.listen()

print("="*50)
print(f"Server listening on {server_socket.getsockname()}")
print("="*50)

received_frames = 0
while True:
    print(f"-- Total frames: {received_frames * (buffer_size // sample_width)}")
    # print("-- Waiting for connection...")

    client_socket, addr = server_socket.accept()
    
    # print("="*50)
    # print("Connected to", addr)

    data = client_socket.recv(buffer_size)
    if data:
        received_frames += 1

    # print("Received:", data.hex())
    # print("="*50)

server_socket.close()
