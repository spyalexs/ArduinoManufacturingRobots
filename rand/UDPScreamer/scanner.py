import socket

LOCAL_IP = socket.gethostbyname("Alexsmen.mshome.net")
CONNECTION_MESSAGE = "I'm a bot! MAC:"

#port for initial connections
CONNECTION_PORT = 5005

#ports for communication to be assigneed
LOWER_COM_PORT = 5010
UPPER_COM_PORT = 5029

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

sock.bind((LOCAL_IP, CONNECTION_PORT))
sock.settimeout(.5)

searching = True
while(searching):
    try:
        message, addr = sock.recvfrom(50)

        if(CONNECTION_MESSAGE in str(message)):
            #trim message to get mac
            macAddress = str(message).split("$$$")[0].split("MAC:")[1]
            print(macAddress)
    except TimeoutError:
        searching = True