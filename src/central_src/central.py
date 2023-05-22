import serial, serial.tools.list_ports
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

    arduinios = [] 
    #wait for bridge connection
    searchStart = time.time()
    while len(arduinios) < 1:
        #get serial ports - connected and unsused
        ports = serial.tools.list_ports.comports()

        for port in ports:
            #go through ports and try to open a serial COM
            try:
                #if arduino connected, save the port
                if("Arduino" in port.description):
                    arduinios.append(port.name)
            except (OSError, serial.SerialException, AttributeError):
                pass

        if(searchStart + 100 < time.time()):
            #cannot find arduinos - timeout
            print("Timeout. Cannot find bridge. Quitting!")
            quit()

        time.sleep(0.1)

    #initialize bridge port - assume the only port found 
    bridge = serial.Serial(arduinios[0])
    #set baud rate to match arduino
    bridge.baudrate = 38400; 
    #set read time
    bridge.timeout = 0.001  

    #create the thread to monitor the serial inbox
    monitorThread = Thread(target=monitor,args=(bridge, queueIn))
    monitorThread.daemon = True
    monitorThread.start()

    #create a thread to send messages out serial
    publisherThread = Thread(target=publish, args=(bridge, queueOut))
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