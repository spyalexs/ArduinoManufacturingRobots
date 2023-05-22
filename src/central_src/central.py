import serial
import sys
import time
import queue
from threading import Thread
from random import random

from monitorSerial import monitor
from publishSerial import publish

def initialize():
    #thread safe queue for messages into central
    queueIn = queue.Queue()
    #thread safe queu for messages out of central
    queueOut = queue.Queue()

    #initialize COM7 port
    COM7 = serial.Serial("COM7")
    #set baud rate to match arduino
    COM7.baudrate = 38400; 
    #set read time
    COM7.timeout = 0.001  

    #create the thread to monitor the serial inbox
    monitorThread = Thread(target=monitor,args=(COM7, queueIn))
    monitorThread.daemon = True
    monitorThread.start()

    #create a thread to send messages out serial
    publisherThread = Thread(target=publish, args=(COM7, queueOut))
    publisherThread.daemon = True
    publisherThread.start()

    return queueIn, queueOut


def cycle():
    #In a cycle:
    #1. in a general cycle, messages in should be handled
    #2. map managment should occur
    #3. any messages (not sent out by other processes) needing sent should be sent


    handleMessagesIn()

    time.sleep(5)
    print("ON")
    SendMessageToBot("bot1", "BUILTIN_LED", 1)    
    time.sleep(5)
    print("Off")
    SendMessageToBot("bot1", "BUILTIN_LED", 0)    

def handleMessagesIn():
    #process incoming messages
    while(not queueIn.empty()):
       print(queueIn.get())

def SendMessageToBot(BotName, Characteristic, Value):
    #botname = string
    #characteristic = string
    #value = int

    #ensure value is a int
    if not (isinstance(Value, int)):
        print("Message failed to send: value is not an int")
        return

    message = BotName + "$" + Characteristic + "$" + str(Value)
    queueOut.put(message)


if __name__ == "__main__":
    if not sys.platform.startswith('win'):
        #ensure windows is being used
        print("This program was designed to run only on a Windows operating system! Quitting.")
        quit()

    #define threads and their queues
    queueIn, queueOut = initialize()

    while(True):
      cycle()