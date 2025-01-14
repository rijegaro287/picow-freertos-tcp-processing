import os
import socket
import wave
import time

buffer_size = 1024

if os.path.exists('./test_data/sine_wave_1khz_out.wav'):
    os.remove('./test_data/sine_wave_1khz_out.wav')

wave_file = wave.open('./test_data/sine_wave_1khz_out.wav', 'w')
n_channels = wave_file.setnchannels(1)
sample_width = wave_file.setsampwidth(2)
sample_rate = wave_file.setframerate(44100)

print("Starting server...")
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

server_socket.bind(("192.168.100.66", 4242))
server_socket.listen()

print("="*50)
print(f"Server listening on {server_socket.getsockname()}")
print("="*50)

received_bytes = 0
start = time.time()
while True:
    elapsed_time = time.time() - start
    if elapsed_time == 0:
        bytes_ps = 0
    else:
        bytes_ps = received_bytes // elapsed_time

    print(f"-- Bytes received: {received_bytes}")
    print(f"-- Speed: {bytes_ps} bytes/s")

    client_socket, addr = server_socket.accept()

    data = client_socket.recv(buffer_size)

    if data:
        wave_file.writeframes(data)
        received_bytes += buffer_size 
