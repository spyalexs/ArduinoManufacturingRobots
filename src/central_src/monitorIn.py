import queue
import socket
import threading

from getConstants import getConnectionPorts

try:
    LOCAL_IP = socket.gethostbyname("Alexsmen.mshome.net")
except socket.gaierror:
    print("Could not find LOCALIP, is the Hotspot/Network up?")
    quit()
    
BUFFER_LENGTH = 50

def launchPacketMonitor(queueIn):
    #queue to pass the kill signal
    killQueue = queue.Queue()

    monitorThread = threading.Thread(target=monitor,args=(queueIn, killQueue))
    monitorThread.daemon = True
    monitorThread.start()

    return killQueue
    
def monitor(queueIn, killQueue):
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
        #sequence that allows program to gracefully exit
        if(not killQueue.empty()):
            break

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


