import serial
import socket

from getConstants import getConnectionPorts

LOCAL_IP = socket.gethostbyname("Alexsmen.mshome.net")
BUFFER_LENGTH = 50

def monitor(queueIn):
    #monitor the updp ports input for messages

    lowerComPort, higherComPort = getConnectionPorts()

    sockets = dict()
    #initialize sockets
    counter = lowerComPort
    while(counter <= higherComPort):
        #setup and bind a socket to the input port
        sockets[str(counter)] = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sockets[str(counter)].settimeout(.005)
        sockets[str(counter)].bind((LOCAL_IP, counter))

        counter += 1

    read = True
    while(read):
        #read each socket
        for key in sockets.keys():
            try:
                message, addr = sockets[key].recvfrom(BUFFER_LENGTH)
                
                if(not str(message) == ""):
                    #get usable part of message
                    messageString = (str(message).split("$$$")[0]).split("b'")[1]

                    #break down into sub messages
                    subMessages = messageString.split("$$")

                    #write out sub messages with the name of the sender attached
                    for subMessage in subMessages:
                        queueIn.put(str(key) + "$" + subMessage)
            except TimeoutError:
                read = True


