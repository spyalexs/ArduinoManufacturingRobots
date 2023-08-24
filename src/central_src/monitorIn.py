import queue
import socket
import threading

from getConstants import getBotConnectionPorts, getStationConnectionPorts, getPacketBufferLength, getRouterHostName, getCentralHostName, getHotSpotHostname


#to find server, first try hotspot, then external network, then fail
try:
    #get hotspot hostname
    LOCAL_IP = socket.gethostbyname(getHotSpotHostname())
    if(LOCAL_IP):
        print("Accepting connections over local hotspot!")
except:
    try:
        LOCAL_IP = socket.gethostbyname(getCentralHostName())
        
        #assume the ip of the router
        ipBytes = LOCAL_IP.split(".")
        ROUTER_IP = str(ipBytes[0]) + "." + str(ipBytes[1]) + "." + str(ipBytes[2]) + ".1"

        ROUTER_NAME = socket.gethostbyaddr(ROUTER_IP)[0]
        if(ROUTER_NAME == getRouterHostName()):
            print("Accepting Connections over switch: " + ROUTER_NAME)
        else:
            print("Invalid switch name: " + str(ROUTER_NAME) + ". Should be: " + str(getRouterHostName()))
            quit()

    except socket.herror:
        print("Could not find LOCALIP, is the environment set up?")
        quit()
        
def launchPacketMonitor(queueIn):
    #queue to pass the kill signal
    killQueue = queue.Queue()

    monitorThread = threading.Thread(target=monitor,args=(queueIn, killQueue))
    monitorThread.daemon = True
    monitorThread.start()

    return killQueue
    
def monitor(queueIn, killQueue):
    #monitor the updp ports input for messages

    lowerComPort, _ = getBotConnectionPorts()
    _, higherComPort = getStationConnectionPorts()

    sockets = dict()
    #initialize sockets
    counter = lowerComPort
    while(counter <= higherComPort):
        #setup and bind a socket to the input port
        sockets[str(counter)] = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sockets[str(counter)].settimeout(.005)
        sockets[str(counter)].bind((LOCAL_IP, counter))

        counter += 1

    bufferLength = getPacketBufferLength()

    read = True
    while(read):
        #sequence that allows program to gracefully exit
        if(not killQueue.empty()):
            break

        #read each socket
        for key in sockets.keys():
            try:
                message, addr = sockets[key].recvfrom(bufferLength)

                if(not str(message) == ""):
                    try:
                        #get usable part of message
                        messageString = (str(message).split("$$$")[0])[2:]

                        #break down into sub messages
                        subMessages = messageString.split("$$")

                        #write out sub messages with the name of the sender attached
                        for subMessage in subMessages:
                            queueIn.put(str(key) + "$" + subMessage)
                    except IndexError:
                        print("Monitor In: index out of range!")
                        print("Message: " + str(message))
            except TimeoutError:
                read = True


