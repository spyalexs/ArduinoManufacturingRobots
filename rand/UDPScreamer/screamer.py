import socket
import time

TARGET_IP = "192.168.137.146"
UDP_PORT = 5005
FREQUENCY = 20

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

i = 0

while(True): 
    message = bytes(str("message:" + str(i) + "$$$$$"), 'utf-8')
 
    sock.sendto(message,(TARGET_IP,UDP_PORT))
    time.sleep(1/FREQUENCY) 

    i += 1
