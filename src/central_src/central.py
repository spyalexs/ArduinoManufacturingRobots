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

def initialize(queueIn, queueOut, queueInGui, queueOutGui, killQueue: queue.Queue):
    #run startup / connection protocol

    #queue to send packets to bots
    queuePacketOut = queue.Queue()

    #queue for new bot connections
    connectionQueue = queue.Queue()

    #create the thread to monitor the serial inbox
    subThreadKills.append(launchPacketMonitor(queueIn))
    
    #create a thread to send messages out serial
    subThreadKills.append(launchPacketPublisher(queueOut, queuePacketOut))

    #launch the connection thread
    subThreadKills.append(launchBotConnector(connectionQueue))

    #if bots were connected during startup
    botConnected = False
    while(queueInGui.empty() and killQueue.empty()):
        #wait for connections until menu closed
        time.sleep(.05)

        while(not connectionQueue.empty()):
            connection = connectionQueue.get()

            #put bot in the startup gui
            queueOutGui.put(connection)

            #get bot info from message
            connectionInfo = str(connection).split("$")

            #add bot overseerer to new connection
            if(len(connectionInfo) >= 6):

                #determine if new connection is bot or station
                if(connectionInfo[4] == "bot"):
                    #set up bot overseer
                    overseers.append(BotOverSeer(connectionInfo[0], connectionInfo[1], connectionInfo[2], queueOut, queuePacketOut, queueOutGui, connectionInfo[5]))

                    #mark that at least one bot was connected
                    botConnected = True
                elif(connectionInfo[4] == "station"):
                    #set up station overseer
                    overseers.append(StationOverSeer(connectionInfo[0], connectionInfo[1], connectionInfo[2], queueOut, queuePacketOut, queueOutGui, connectionInfo[5]))
                else:
                    #something really isn't right
                    print("Cannot connect to type: " + connectionInfo[4])

            
            else:
                #somethings not right...
                print("Invalid connection detected: " + connection)

    #see what to do next based on menu result
    startUpAction = queueInGui.get()

    if(str(startUpAction) == "Stop" or botConnected == False):
        #if stop then return - kinda crashes but thats cool cause it should just stop
        return

    time.sleep(1.0)

    #create Gui

    return queueIn, queueOut, queueInGui, queueOutGui


def cycle(queueIn, queueOut, queueInGui, queueOutGui):
    #In a cycle:
    #1. messages in should be handled
    #2. gui requests in should be handled
    #3. map managment should occur
    #4. robots commands should be checked
    #5. any messages (not sent out by other processes) needing sent should be sent

    handleMessagesIn(queueIn, queueOutGui) #1
    
    handleGUIIn(queueInGui) #2

    for overseer in overseers: #3
        overseer.cycle()

    time.sleep(.05)

def handleMessagesIn(queueIn: queue.Queue, queueOutGui: queue.Queue):
    #process incoming messages - from bridge/bots
    while(not queueIn.empty()):
       message = queueIn.get()
        #print(message)
       handleBotMessage(message, queueOutGui, overseers)

def handleGUIIn(queueInGui: queue.Queue):
    #processes messages from the GUI
    while(not queueInGui.empty()):
        message = queueInGui.get()
        #handle all messages in the queu
        if message.m_direct == True:
            #if the message is ment to go directly to the robot
            for overseer in overseers:
                if overseer.m_port == message.getDirectString()[0]:
                    overseer.sendMessageToOverseen(message.getDirectString()[1])
        else:

            #find correct overseer to handle message
            targetOverseer = None
            for overseer in overseers:
                if(overseer.m_port == message.m_target):
                    targetOverseer = overseer

            if(targetOverseer == None and not message.m_target == "All"):
                print("Invalid GUI Message, target: " + message.m_target + " cannot be found!")
                return 

            #if the message should first be controller processed
            if message.m_characteristic == "commandSequence":
                
                # find the bot it belongs to and give the commands
                    targetOverseer.issueRoute(message.m_value)

            elif message.m_characteristic == "commandIssue":
                # find the bot it belongs to and give the command

                targetOverseer.issueCommand(message.m_value)

            elif message.m_characteristic == "locationSet":
                #set the location for of the robot in the locatalization system

                targetOverseer.setLocation(message.m_value) 

            elif message.m_characteristic == "ready":
                if(message.m_value == True):
                    #if the GUI has sent to central that it is ready

                    #have each overseerer send an initial status
                    for overseer in overseers:
                        overseer.sendInitialStateToGUI()

            elif message.m_characteristic == "SetStationItem":
                #gui has sent to set the item in a station

                    if(targetOverseer.m_type == "station"):
                        targetOverseer.setStationItem(message.m_value)
                    else:
                        print("Cannot set item type on: " + targetOverseer.m_type)

            elif message.m_characteristic == "DispenseStationItem":
                #gui has reqested that an item be transfered to bot

                if(targetOverseer.m_type == "station" and targetOverseer.m_port == message.m_target):
                    targetOverseer.dispenseItem(message.m_value)
                else:
                    print("Cannot dispense item from: " + targetOverseer.m_type)

            elif message.m_characteristic == "CollectStationItem":
                #gui has requested that an item be transfered from bot

                    if(targetOverseer.m_type == "station"):
                        targetOverseer.collectItem(message.m_value)
                    else:
                        print("Cannot collect item on: " + targetOverseer.m_type)

            elif message.m_characteristic == "SetItemCapacity":
                #gui that bot capacity has been sent
                
                if(targetOverseer.m_type == "bot"):
                    #set the number of items in the overseer
                    targetOverseer.setNumberOfItemSlots(message.m_value)
                else:
                    print("Cannot set item capacity on type: " + targetOverseer.m_type)

            elif message.m_characteristic == "AddItem":
                #gui has requested that an item be added to the bot

                if(targetOverseer.m_type == "bot"):
                    #set the number of items in the overseer
                    targetOverseer.addItem(message.m_value)
                else:
                    print("Cannot add item to type: " + targetOverseer.m_type)

            elif message.m_characteristic == "RemoveItem":
                #gui has requested that an item be removed from the bot

                if(targetOverseer.m_type == "bot"):
                    #set the number of items in the overseer
                    targetOverseer.removeItem(message.m_value)
                else:
                    print("Cannot remove item from type: " + targetOverseer.m_type)
            else:
                #note the message characteristic cannot be processed
                print("Cannot process message, unhandled characteristic: " + message.m_characteristic)

                
                 
def sendMessageToBot(BotName, Characteristic, Value, queueOut: queue.Queue):
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

def sendStringToBot(string, queueOut: queue.Queue):
    #put a string in the output queue - be sure that the string wil be recognized by the bridge
    queueOut.put(string)

def runController(queueIn: queue.Queue, queueOut: queue.Queue, queueInGui: queue.Queue, queueOutGui: queue.Queue, controllerKillQueue: queue.Queue): 
    #kill signal for main controller

    subThreadKills.append(controllerKillQueue)

    #start up the program
    initialize(queueIn, queueOut, queueInGui, queueOutGui, controllerKillQueue)

    while(controllerKillQueue.empty()):
        cycle(queueIn, queueOut, queueInGui, queueOutGui)


