import socket
import queue
import threading
from getConstants import getListeningPort, getHotSpotHostname, getCentralHostName

def launchPacketPublisher(queueOut, queuePacketOut):
    #queue to pass the kill signal to the thread
    killQueue = queue.Queue()

    publisherThread = threading.Thread(target=publish, args=(queueOut, queuePacketOut, killQueue))
    publisherThread.daemon = True
    publisherThread.start()

    return killQueue


def publish(queueOut, queuePacketOut, killQueue):
    #COM - the serial port to be published to
    #queueOut - the queue to publish messages from

    listeningPort = getListeningPort()
    outSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    #determine local ip
    try:
        #try looking as hotspot
        LOCAL_IP = socket.gethostbyname(getHotSpotHostname())
    except:
        try:
            #try looking for ip on other network
            LOCAL_IP = socket.gethostbyname(getCentralHostName())
        except:
            print("Publisher cannot find LOCAL IP!, quitting!")
            quit()


    while(True):
        #sequence that allows program to gracefully exit
        if(not killQueue.empty()):
            break

        if not queueOut.empty():
            try:
                #get message from a queue
                messageOut = queueOut.get()
                messageArray = messageOut.split(":")
                if(len(messageArray) >=2):
                    address = messageArray[0]
                    message = messageArray[1]
   
                    print("Message to bot: " + str(message))

                    messageBytes = bytes(message + "\0", "utf-8")

                    if(len(messageBytes) >= 49):
                        print("Cannot publish")


                    if(address ==LOCAL_IP):
                        #the address is a port number so send locally to sim
                        outSocket.sendto(messageBytes, (LOCAL_IP, address))
                    else:
                        #address is an ip so send to bot
                        outSocket.sendto(messageBytes, (address,listeningPort))

                else:
                    #print messages without address
                    print("Invalid Message: " + messageOut)
            except queue.Empty:
                #this should never happen...
                print("Queue unexpectently went empty... odd.")
                break

        if not queuePacketOut.empty():
            #if there is a packet that needs sent to the bot

            try:
                #get packet from queue
                packetTuple = queuePacketOut.get()

                #these come pre packaged
                outSocket.sendto(packetTuple[1], (packetTuple[0][0], listeningPort))

            except queue.Empty:
                #this should never happen...
                print("Queue unexpectently went empty... odd.")
                break

    print("The matrix has broken!")

        

            


