import serial, serial.tools.list_ports
import sys
import time
import queue
from threading import Thread
from random import random

from monitorIn import monitor
from publishToBot import publish
from handleMessage import handleBotMessage
from gui.launchGUI import launchGUI
from execution.BotOverSeer import BotOverSeer
from execution.connectBots import launchBotConnector

overseerers = [] # a list of commander classes that tell robots what to do

#this is the main thread for the central controller

def initialize():
    #run startup / connection protocol

    #thread safe queue for messages into central
    queueIn = queue.Queue()
    #thread safe queu for messages out of central
    queueOut = queue.Queue()

    #queue for incomming gui requests - from GUI to central
    queueInGUI = queue.Queue()
    #queue for outgoing gui messages - from central to GUI
    queueOutGUI = queue.Queue()

    #queue for new bot connections
    connectionQueue = queue.Queue()

    #create the thread to monitor the serial inbox
    monitorThread = Thread(target=monitor,args=(queueIn,))
    monitorThread.daemon = True
    monitorThread.start()
    
    #create a thread to send messages out serial
    publisherThread = Thread(target=publish, args=(queueOut,))
    publisherThread.daemon = True
    publisherThread.start()

    #launch the gui
    launchGUI(queueInGUI, queueOutGUI)
    #launch the connection thread
    launchBotConnector(connectionQueue)

    connectedRobots = []
    while(queueInGUI.empty()):
        #wait for connections until menu closed
        time.sleep(.05)

        while(not connectionQueue.empty()):
            connection = connectionQueue.get()
            queueOutGUI.put(connection)

            #get bot info from message
            connectionInfo = str(connection).split("$")

            #add bot overseerer to new connection
            if(len(connectionInfo) >= 3):
                overseerers.append(BotOverSeer(connectionInfo[0], connectionInfo[1], connectionInfo[2], queueOut, queueOutGUI))
                connectedRobots.append(connectionInfo[0])

    #see what to do next based on menu result
    startUpAction = queueInGUI.get()

    if(str(startUpAction) == "Stop" or len(connectedRobots) == 0):
        #if stop then return - kinda crashes but thats cool cause it should just stop
        return

    time.sleep(1.0)

    #create Gui

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
        #print(message)
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
                    if overseer.m_port == message.m_target:
                        overseer.issueCommandSequences(message.m_value)

            if message.m_characteristic == "commandIssue":
                # find the bot it belongs to and give the command

                print(message.m_target)

                for overseer in overseerers:
                    if overseer.m_port == message.m_target:
                        overseer.issueCommandSequences([message.m_value])
                 
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