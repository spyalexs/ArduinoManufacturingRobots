import socket
import queue
import time
from getConstants import getListeningPort

def publish(queueOut):
    #COM - the serial port to be published to
    #queueOut - the queue to publish messages from

    listeningPort = getListeningPort()
    outSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    while(True):
        if not queueOut.empty():
            try:
                #get message from a queue
                messageOut = queueOut.get()
                messageArray = messageOut.split(":")
                if(len(messageArray) >=2):
                    address = messageArray[0]
                    message = messageArray[1]
   
                    messageBytes = bytes(message, "utf-8")

                    #write the message out to the bot
                    outSocket.sendto(messageBytes, (address,listeningPort))

                else:
                    #print messages without address
                    print("Invalid Message: " + messageOut)
            except queue.Empty:
                #this should never happen...
                print("Queue unexpectently went empty... odd.")
                break


    while(True):
        print("The matrix has broken!")

        

            


