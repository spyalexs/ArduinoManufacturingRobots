import signal
import sys
import os
import time
import queue
from threading import Thread
from random import random

from monitorIn import launchPacketMonitor
from publishToBot import launchPacketPublisher
from handleMessage import handleBotMessage
from gui.launchGUI import launchGUI
from execution.BotOverSeer import BotOverSeer
from execution.OverSeer import OverSeer
from execution.StationOverSeer import StationOverSeer
from execution.connectBots import launchBotConnector

overseers: OverSeer = [] # a list of commander classes that tell robots what to do
subThreadKills = [] #a list of all the thread PIDS

#this is the main thread for the central controller

def initialize():
    #run startup / connection protocol

    #thread safe queue for messages into central
    queueIn = queue.Queue()
    #thread safe queu for messages out of central
    queueOut = queue.Queue()
    #queue to send packets to bots
    queuePacketOut = queue.Queue()

    #queue for incomming gui requests - from GUI to central
    queueInGUI = queue.Queue()
    #queue for outgoing gui messages - from central to GUI
    queueOutGUI = queue.Queue()

    #queue for new bot connections
    connectionQueue = queue.Queue()

    #create the thread to monitor the serial inbox
    subThreadKills.append(launchPacketMonitor(queueIn))
    
    #create a thread to send messages out serial
    subThreadKills.append(launchPacketPublisher(queueOut, queuePacketOut))

    #launch the gui
    subThreadKills.append(launchGUI(queueInGUI, queueOutGUI))
    #launch the connection thread
    subThreadKills.append(launchBotConnector(connectionQueue))

    #if bots were connected during startup
    botConnected = False
    while(queueInGUI.empty()):
        #wait for connections until menu closed
        time.sleep(.05)

        while(not connectionQueue.empty()):
            connection = connectionQueue.get()

            #put bot in the startup gui
            queueOutGUI.put(connection)

            #get bot info from message
            connectionInfo = str(connection).split("$")

            #add bot overseerer to new connection
            if(len(connectionInfo) >= 5):

                #determine if new connection is bot or station
                if(connectionInfo[4] == "bot"):
                    #set up bot overseer
                    overseers.append(BotOverSeer(connectionInfo[0], connectionInfo[1], connectionInfo[2], queueOut, queuePacketOut, queueOutGUI))

                    #mark that at least one bot was connected
                    botConnected = True
                elif(connectionInfo[4] == "station"):
                    #set up station overseer
                    overseers.append(StationOverSeer(connectionInfo[0], connectionInfo[1], connectionInfo[2], queueOut, queuePacketOut, queueOutGUI))
                else:
                    #something really isn't right
                    print("Cannot connect to type: " + connectionInfo[4])

            
            else:
                #somethings not right...
                print("Invalid connection detected: " + connection)

    #see what to do next based on menu result
    startUpAction = queueInGUI.get()

    if(str(startUpAction) == "Stop" or botConnected == False):
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

    for overseer in overseers: #3
        overseer.cycle()

    time.sleep(.05)

def handleMessagesIn():
    #process incoming messages - from bridge/bots
    while(not queueIn.empty()):
       message = queueIn.get()
        #print(message)
       handleBotMessage(message, queueOutGui, overseers)

def handleGUIIn():
    #processes messages from the GUI
    while(not queueInGUI.empty()):
        message = queueInGUI.get()
        #handle all messages in the queu
        if message.m_direct == True:
            #if the message is ment to go directly to the robot
            for overseer in overseers:
                if overseer.m_port == message.getDirectString()[0]:
                    overseer.sendMessageToOverseen(message.getDirectString()[1])
        else:
            #if the message should first be controller processed
            if message.m_characteristic == "commandSequence":
                
                # find the bot it belongs to and give the commands
                for overseer in overseers:
                    if overseer.m_port == message.m_target:
                        overseer.issueRoute(message.m_value)

            if message.m_characteristic == "commandIssue":
                # find the bot it belongs to and give the command

                for overseer in overseers:
                    if overseer.m_port == message.m_target:
                        overseer.issueCommand(message.m_value)

            if message.m_characteristic == "locationSet":
                #set the location for of the robot in the locatalization system

                for overseer in overseers:
                    if overseer.m_port == message.m_target:
                        overseer.setLocation(message.m_value) 

            if message.m_characteristic == "ready":
                if(message.m_value == True):
                    #if the GUI has sent to central that it is ready

                    #have each overseerer send an initial status
                    for overseer in overseers:
                        overseer.sendInitialStateToGUI()

            if message.m_characteristic == "SetStationItem":
                #gui has sent to set the item in a station

                for overseer in overseers:
                    if(overseer.m_type == "station" and overseer.m_port == message.m_target):
                        overseer.setStationItem(message.m_value)
                    else:
                        if(overseer.m_port == message.m_target):
                            print("Cannot set item type on: " + overseer.m_type)

            if message.m_characteristic == "DispenseStationItem":
                #gui has reqested that an item be transfered to bot

                for overseer in overseers:
                    if(overseer.m_type == "station" and overseer.m_port == message.m_target):
                        overseer.dispenseItem(message.m_value)
                    else:
                        if(overseer.m_port == message.m_target):
                            print("Cannot dispense item from: " + overseer.m_type)

            if message.m_characteristic == "CollectStationItem":
                #gui has requested that an item be transfered from bot

                for overseer in overseers:
                    if(overseer.m_type == "station" and overseer.m_port == message.m_target):
                        overseer.collectItem(message.m_value)
                    else:
                        if(overseer.m_port == message.m_target):
                            print("Cannot collect item on: " + overseer.m_type)

                
                 
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

def cleanKill(signum, frame):
    print("Cleanly shutting down!")

    for threadQueue in subThreadKills:
        #add something to the kill queue so the thread knows its time to end
        threadQueue.put("Kill")

    quit()


if __name__ == "__main__":
    #redefine ctrl+c to cleanly kill the program... otherwise some processes like to hang around
    signal.signal(signal.SIGINT, cleanKill)

    if not sys.platform.startswith('win'):
        #ensure windows is being used
        print("This program was designed to run only on a Windows operating system! Quitting.")
        quit()

    #define threads and their queues
    queueIn, queueOut, queueInGUI, queueOutGui = initialize()

    while(True):
        #main cycle for the central controller thread - Gui/publishers/subscribers are threaded differently
      cycle()