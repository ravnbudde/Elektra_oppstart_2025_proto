import socket

# UDP-socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_socket.bind(("0.0.0.0", 15000))  # Lytt på alle tilgjengelige nettverksadaptere

print("Lytter på UDP-port 15000...")

while True:
    # Mottar data fra Arduino
    data, addr = server_socket.recvfrom(1024)  # 1024 byte buffer
    print(f"Mottok melding: {data.decode()}")

