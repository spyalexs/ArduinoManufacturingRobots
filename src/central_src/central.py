import serial, serial.tools.list_ports
import sys
import time
import queue
from threading import Thread
from random import random

from monitorSerial import monitor
from publishSerial import publish
from handleMessage import handleBotMessage
from gui.launchGUI import launchGUI
from execution.BotOverSeer import BotOverSeer

overseerers = [] # a list of commander classes that tell robots what to do

#this is the main thread for the central controller

def initialize():
    #thread safe queue for messages into central
    queueIn = queue.Queue()
    #thread safe queu for messages out of central
    queueOut = queue.Queue()
    #queue for incomming gui requests - from GUI to central
    queueInGUI = queue.Queue()
    #queue for outgoing gui messages - from central to GUI
    queueOutGUI = queue.Queue()

    #create Gui
    launchGUI(queueInGUI, queueOutGUI)

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

    #TODO - have bridge communicate number of connections to determine the number of overseers
    overseerers.append(BotOverSeer("bot1", queueOut))

    return queueIn, queueOut, queueInGUI, queueOutGUI


def cycle():
    #In a cycle:
    #1. messages in should be handled
    #2. gui requests in should be handled
    #3. map managment should occur
    #4. robots commands should be checked
    #5. any messages (not sent out by other processes) needing sent should be sent

    handleMessagesIn() #1
    
    handleGUIIn() #2

    for overseer in overseerers: #3
        overseer.cycle()

    time.sleep(.05)

def handleMessagesIn():
    #process incoming messages - from bridge/bots
    while(not queueIn.empty()):
       message = queueIn.get()
       print(message)
       handleBotMessage(message, queueOutGui, overseerers)

def handleGUIIn():
    #processes messages from the GUI
    while(not queueInGUI.empty()):
        message = queueInGUI.get()
        #handle all messages in the queu
        if message.m_direct == True:
            #if the message is ment to go directly to the robot
            sendStringToBot(message.getDirectString())
        else:
            #if the message should first be controller processed
            if message.m_characteristic == "commandSequence":
                
                # find the bot it belongs to and give the commands
                for overseer in overseerers:
                    if overseer.m_name == message.m_target:
                        overseer.issueCommandSequences(message.m_value)
                

def sendMessageToBot(BotName, Characteristic, Value):
    #botname = string
    #characteristic = string
    #value = int

    #ensure value is a int
    if not (isinstance(Value, int)):
        print("Message failed to send: value is not an int")
        return

    #formatting that the bridge can translate into a characteristic
    message = BotName + "$" + Characteristic + "$" + str(Value)
    queueOut.put(message)

def sendStringToBot(string):
    #put a string in the output queue - be sure that the string wil be recognized by the bridge
    queueOut.put(string)


if __name__ == "__main__":
    if not sys.platform.startswith('win'):
        #ensure windows is being used
        print("This program was designed to run only on a Windows operating system! Quitting.")
        quit()

    #define threads and their queues
    queueIn, queueOut, queueInGUI, queueOutGui = initialize()

    while(True):
        #main cycle for the central controller thread - Gui/publishers/subscribers are threaded differently
      cycle()