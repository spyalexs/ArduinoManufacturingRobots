import socket
from threading import Thread
import queue
import time

from getConstants import getConnectionPorts, getPacketBufferLength

try:
    LOCAL_IP = socket.gethostbyname("Alexsmen.mshome.net")
except socket.gaierror:
    print("Could not find LOCALIP, is the Hotspot/Network up?")
    quit()

BOT_CONNECTION_MESSAGE = "Im a bot! MAC:"
STATION_CONNECTION_MESSAGE = "Im a station! MAC:"
CONNECTION_REPLY = "Connection Established on:"
REPLY_FREQUENCY = 20 #Hz
CONNECTION_TIMEOUT = 5 #seconds

#port for initial connections
CONNECTION_PORT = 5006

#bot listening port
BOT_LISTENING_PORT = 5005

def launchBotConnector(connectorQueue):
    #start a thread to manage bot connnection

    killQueue = queue.Queue()

    connectorThread = Thread(target=connectBots, args=(connectorQueue, killQueue))
    connectorThread.isDaemon = True
    connectorThread.start()

    #return thread id
    return killQueue


def connectBots(connectorQueue, killQueue):
    lowerComPort, higherComPort = getConnectionPorts()
    #create a dictionary of known MAC addresses and the ports the bot has been assigned to
    #allows for reconnections
    knownMACs = dict()

    #setup socket for connection port - sockets for coms are in bot handlers
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    sock.bind((LOCAL_IP, CONNECTION_PORT))
    sock.settimeout(.01)

    #the next port to be assigned
    nextPort = lowerComPort

    bufferLength = getPacketBufferLength()

    searching = True
    while(searching):

        #sequence that allows program to gracefully exit
        if(not killQueue.empty()):
            break

        #run until entire system is down
        try:
            message, addr = sock.recvfrom(bufferLength)
            if(BOT_CONNECTION_MESSAGE in str(message) or STATION_CONNECTION_MESSAGE in str(message)):
                #trim message to get mac
                macAddress = str(message).split("$$$")[0].split("MAC:")[1]

                if(macAddress in knownMACs.keys()):
                    #the device is already connected

                    #determine if the connection is from a bot or station
                    if(BOT_CONNECTION_MESSAGE in str(message)):
                        #bot connection
                        print("Bot --- Reconnection from: " + macAddress + ". Reassigning to port: " + str(knownMACs[macAddress]))
                    else:
                        #station connection
                        print("Bot --- Reconnection from: " + macAddress + ". Reassigning to port: " + str(knownMACs[macAddress]))


                    #reply back with connection port
                    message = bytes(CONNECTION_REPLY + str(knownMACs[macAddress]) +"$$$", 'utf-8')
                    sock.sendto(message, (addr[0], BOT_LISTENING_PORT))

                    #stop any duplicate connections
                    time.sleep(1.0)

                    #flush the socket
                    clearing = True
                    while(clearing == True):
                        try:
                            sock.recv(bufferLength)
                        except TimeoutError:
                            clearing = False

                elif(nextPort <= higherComPort):
                    #new connection
                    knownMACs[macAddress] = nextPort

                    #determine wether connection is from a bot or station
                    if(BOT_CONNECTION_MESSAGE in str(message)):
                        #connection from bot
                        print("Bot --- Connection from: " + macAddress + ". Assigning to port: " + str(knownMACs[macAddress]))

                        #send back to main thread to be added to bot overseer
                        connectorQueue.put(macAddress + "$" + str(nextPort) + "$" + str(addr[0]) + "$" + str(addr[1]) + "$bot")

                    else:
                        #connection from station
                        print("Station --- Connection from: " + macAddress + ". Assigning to port: " + str(knownMACs[macAddress]))

                        #send back to main thread to be added to bot overseer
                        connectorQueue.put(macAddress + "$" + str(nextPort) + "$" + str(addr[0]) + "$" + str(addr[1]) + "$station")

                    #send back connection string
                    message = bytes(CONNECTION_REPLY + str(nextPort) +"$$$", 'utf-8')
                    sock.sendto(message, (addr[0], BOT_LISTENING_PORT))

                    nextPort += 1

                    #stop any duplicate connections
                    time.sleep(1.0)

                    #flush the socket
                    clearing = True
                    while(clearing == True):
                        try:
                            sock.recv(bufferLength)
                        except TimeoutError:
                            clearing = False

                else:
                    print("Connection found but cannot establish: Out of valid COM ports.")


        except TimeoutError:
            searching = True #make the try clause happy


if(__name__ == "__main__"):
    connectBots(queue.Queue())