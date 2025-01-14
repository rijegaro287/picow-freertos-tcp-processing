import wave
import socket
import os
import time

if(os.path.exists('./test_data/sine_wave_1khz_out.wav')):
    os.remove('./test_data/sine_wave_1khz_out.wav')

wave_file = wave.open('./test_data/sine_wave_1khz.wav', 'rb')
n_channels = wave_file.getnchannels()
n_samples = wave_file.getnframes()
sample_width = wave_file.getsampwidth()
sample_rate = wave_file.getframerate()
buffer_size = 128

print(f'Number of samples: {n_samples}')
print(f'Sample width: {sample_width} bytes')
print(f'Sample rate: {sample_rate}')

ip_addr = "192.168.100.70"
port = 4242

total_retries = 0
total_frames = 0

SEND_NEXT = '1'

start = time.time()
for i in range(0, n_samples, buffer_size // sample_width):
    print(f'-- Total frames: {total_frames * (buffer_size // sample_width)}')
    print(f'-- Total retries: {total_retries}')

    frames = wave_file.readframes(buffer_size // sample_width)
    # print(f'-- Frames:\n{frames.hex()}')

    frame_retries = 0
    response = ''
    while response != SEND_NEXT:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_socket.connect((ip_addr, port))

        client_socket.send(frames)
        client_socket.settimeout(3)
        
        response = client_socket.recv(buffer_size).decode()
        
        print(f'-- Response: {response}')

        if not response:
            print('-- Connection closed by server')
            break

        if response == SEND_NEXT:
            total_frames += 1
        else:
            frame_retries += 1

        client_socket.settimeout(None)
        client_socket.close()

    total_retries += frame_retries

end = time.time()

print(f'Elapsed time: {end - start} seconds')

wave_file.close()
