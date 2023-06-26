import socket

TARGET_IP = "192.168.137.1"
UDP_PORT = 5005

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

sock.bind((TARGET_IP, UDP_PORT))
sock.settimeout(5)

while(True):
    print("waiting")
    try:
        message, addr = sock.recvfrom(50)
        print(str(message))

    except TimeoutError:
        print("Timeout")

