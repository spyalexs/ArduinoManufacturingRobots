import serial
import queue
import time

def publish(COM, queueOut):
    #COM - the serial port to be published to
    #queueOut - the queue to publish messages from

    while(True):
        if not queueOut.empty():
            try:
                #get message from a queue
                messageOut = queueOut.get()
                print(messageOut)
            except queue.Empty:
                #this should never happen...
                print("Queue unexpectently went empty... odd.")
                break
            
            print(messageOut)

            #write the message out to the bot
            COM.write(bytes(messageOut, "utf-8"))
            time.sleep(.01)

    while(True):
        print("The matrix has broken!")

        

            


