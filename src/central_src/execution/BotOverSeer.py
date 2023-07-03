import time

from gui.GUIOutMessage import GUIOutMessage
from getConstants import getCommandLocalizationEffects

class BotOverSeer:
    # a class to monitor an individual bot's function on the controller end
    
    # for now, the bot overseer will be overriden by direct to bot messages!
    m_directToBotOverride = True

    m_currentLocation = "Node1-A" # the current location of the robot, starts at a default
    m_localizing = False # if the robot is currently being localized by the controller


    m_patience = .5 # how long to wait before attempting to reissue a command
    m_lastIssue = None
    m_written = False
    m_confirmed = False
    m_messageNumber = 1

    m_status = 0 # keeps track of the bot's status

    m_commands = [] # list of raw commands to send to bot - ment for manually sending commands
    m_route = [] # list of steps sent to the bot - meant for going through a route
    
    m_connectionTimeout = 5 # after five seconds of not hearing from bot, assume connection issues
    m_connected = False
    
    def __init__(self, macAddress, port, ip_address, queueToBots, queueToGUI):
        self.m_port = port
        self.m_mac = macAddress
        self.m_ip = ip_address
        self.m_queue = queueToBots
        self.m_queueGui = queueToGUI

        self.m_lastIssue = 0

        self.m_lastConnection = time.time()
        self.m_localizationEffects = getCommandLocalizationEffects()

    def updateStatus(self, status):
        self.m_status = status

        print("My status is: " + str(status))

        #send status to gui
        self.sendStatusToGui(status)

        #also clear the patience timer so next command is immediate
        self.m_lastIssue = 0

        #update the connection on the gui
        self.sendConnectionStatusToGui()

    def externalSentMessage(self, characteristic, value):
        #TODO - implement this via publisher

        #lets the overseer know that a direct-to-bot message was sent
        if(self.m_directToBotOverride):
            self.m_commands = [] # clear commands to prevent confusion
            return True # allow the message to pass through
        else:
            if(not self.m_localizing):
                #if the controller is not localizing the robot
                return True
            
            if ((characteristic == "commandIssue") and (value == 255)):
                #always allow abort commands
                return True 
            
            #if doing nothing allow command to pass through
            return False # block the message - only messages from the overseer may reach the bot


    def issueCommand(self, command):
        #if no command sequence already being ran, add a sequence to be ran

        #special abort case
        if(command == 255):
            #the current sequence needs to be aborted 
            self.sendMessageToBot(self.m_port + "$commandIssue$255")
            
            #note that this breaks the localization tracking
            self.m_localizing = False
            self.sendLocalizationStatusToGui()

            return

        if len(self.m_commands and self.m_route) == 0:
            self.m_commands = [command]

            #figure out and implement the effect on the localization system
            #NOTE: commands that do not break localization should be processed as routes as not to break the localization
            if(self.m_localizationEffects[command] == -1 or self.m_localizationEffects[command] == 1):
                #if localization is broken
                self.m_localizing = False
                self.sendLocalizationStatusToGui()

            print("New command added: " + str(command))
        else:
            print("Failed to add new command to bot: " + str(self.m_port) + ". The command/route queue is already full.")


    def issueRoute(self, route):
        if(not self.m_localizing):
            print("Cannot execute route, robot: " + self.m_port + " is not localizing. Set location to begin localizing")

            #TODO: make GUI flash when this happens
            return
        
        #if no command sequence already being ran, add a sequence to be ran

        if len(self.m_commands) == 0:
            self.m_commands = route

            print("New route added: " + str(route))
        else:
            print("Failed to add new command to bot: " + str(self.m_port) + ". The command/route queue is already full.")



    def cycle(self):
        #check connection status
        if(self.m_lastConnection + self.m_connectionTimeout < time.time()):
            if(self.m_connected):
                self.m_connected = False

                #tell the gui to update the connection only if it has changed
                self.sendConnectionStatusToGui()

            #if disconnected, clear all commands
            self.m_commands = []
                                    
            self.m_written = False
            self.m_confirmed = False
            self.updateStatus(0)

        #check the bots status
        if(self.m_status == 255):
            #if a command was aborted
            self.m_commands = [] # clear all commands
        elif(self.m_status == 254) or (self.m_status == 0):

            if(len(self.m_commands) > 0):
                #if a command needs written

                if(not self.m_written):
                    self.m_written = True # mark so the system knows if the systemhas already written a command
                    self.m_confirmed = False #command has not yet been confirmed

                    #write command to bot
                    self.sendMessageToBot(self.m_port + "$commandIssue$" + str(self.m_commands[0]))
        elif(self.m_status == 253):
            # if the status is 253, the robot is waiting for confirmation to run the command - essential for ensure commands do not get issued twice
            # sorta - kinda a handshake

            #if the robot is waiting or doing nothing
            self.m_written = False

            if(not self.m_confirmed):
                self.m_confirmed = True

                #issue confirmation
                self.sendMessageToBot(self.m_port + "$commandIssue$" + str(253))
        else:
            if(self.m_written):
                #if the command had been attempted to been written and now has been

                self.m_written = False
                self.m_confirmed = False
                self.m_commands.pop(0)

    def sendStatusToGui(self, status):
        #put a gui message in the GUI queue
        message = GUIOutMessage(self.m_port, "commandStatus", status)
        self.m_queueGui.put(message)

    def sendConnectionStatusToGui(self):
        #put a gui message in the GUI queue
        message = GUIOutMessage(self.m_port, "connectionStatus", int(self.m_connected))
        self.m_queueGui.put(message)

    def sendLocalizationStatusToGui(self):
        #put a localization status message in the the GUI queue
        message = GUIOutMessage(self.m_port, "localizationStatus", int(self.m_localizing))
        self.m_queueGui.put(message)

    def sendMessageToBot(self, message):
        #send a message to the bot
        self.m_queue.put(self.m_ip + ":" + message)

    def setLocation(self, location):
        #the current location of the robot
        self.m_currentLocation = location

        #mark the robot as now localizing
        self.m_localizing = True
        
        #tell the gui that the robot is now localizing
        self.sendLocalizationStatusToGui()

    def connectionHeard(self):
        if(not self.m_connected):
            self.m_connected = True

            #tell the gui to update the connection only if it has changed
            self.sendConnectionStatusToGui()

        #log that a connection was heard from the bot
        self.m_lastConnection = time.time()
            




            

        
    