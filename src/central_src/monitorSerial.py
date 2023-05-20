import serial
import queue
import time

def monitor(COM, queueIn):
    #monitor the serial input for messages

    while(True):
    #COM - the serial port to be monitored
    #queueIn - the queue to store messages recieved to
        message = COM.readline()
        #ignore empty messages
        if(message == """b''"""): 
            message = None

        if(message):
            #remove line endings
            message = message[:-2]

            #decode from bytes
            message = message.decode("utf-8")  


            #add message to the queue for the main thread to find if signalled
            if("$$$$$" == message[:5]):
                queueIn.put(message[5:])
            else:
                #harmlessly spit out the message
                print("Bridge: " + message)
    
