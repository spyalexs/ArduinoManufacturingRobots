import time
from gui.GUIOutMessage import GUIOutMessage

class BotOverSeer:
    # a class to monitor an individual bot's function on the controller end
    
    # for now, the bot overseer will be overriden by direct to bot messages!
    m_directToBotOverride = True
    m_localizing = False # if the robot is currently being localized by the controller

    m_patience = 0.5 # how long to wait before attempting to reissue a command
    m_lastIssue = None
    m_attemptingToWrite = False

    m_status = 0 # keeps track of the bot's status
    m_commands = []
    
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


    def issueCommandSequences(self, sequence):
        #if no command sequence already being ran, add a sequence to be ran

        if len(self.m_commands) == 0:
            self.m_commands = sequence

            print("New command sequence added: " + str(sequence))
        else:
            print("Failed to add new command sequence to bot: " + str(self.m_port) + ". The sequence is already full.")

    def cycle(self):
        #check connection status
        if(self.m_lastConnection + self.m_connectionTimeout < time.time()):
            if(self.m_connected):
                self.m_connected = False

                #tell the gui to update the connection only if it has changed
                self.sendConnectionStatusToGui()


        #check the bots status
        if(self.m_status == 255):
            #if a command was aborted
            self.m_commands = [] # clear all commands
        elif(self.m_status == 254) or (self.m_status == 0):

            if(len(self.m_commands) > 0):
                #if a command needs written

                self.m_attemptingToWrite = True # mark so the system knows if the system is attempting to write a command

                #if the robot is waiting or doing nothing
                if(time.time() > self.m_lastIssue + self.m_patience):
                    self.m_lastIssue = time.time()

                    #write command to bot
                    self.sendMessageToBot(self.m_port + "$commandIssue$" + str(self.m_commands[0]))
        elif(self.m_status == 253):
            # if the status is 253, the robot is waiting for confirmation to run the command - essential for ensure commands do not get issued twice
            # sorta - kinda a handshake

            #if the robot is waiting or doing nothing
            if(time.time() > self.m_lastIssue + self.m_patience):
                self.m_lastIssue = time.time()

                #issue confirmation
                self.sendMessageToBotF(self.m_port + "$commandIssue$" + str(253))
        else:
            if(self.m_attemptingToWrite):
                #if the command had been attempted to been written and now has been
                self.m_attemptingToWrite = False
                self.m_commands.pop(0)

    def sendStatusToGui(self, status):
        #put a gui message in the GUI queue
        message = GUIOutMessage(self.m_port, "commandStatus", status)
        self.m_queueGui.put(message)

    def sendConnectionStatusToGui(self):
        #put a gui message in the GUI queue
        message = GUIOutMessage(self.m_port, "connectionStatus", int(self.m_connected))
        self.m_queueGui.put(message)

    def sendMessageToBot(self, message):
        #send a message to the bot
        self.m_queue.put(self.m_ip + ":" + message)

    def connectionHeard(self):
        if(not self.m_connected):
            self.m_connected = True

            #tell the gui to update the connection only if it has changed
            self.sendConnectionStatusToGui()

        #log that a connection was heard from the bot
        self.m_lastConnection = time.time()
            




            

        
    