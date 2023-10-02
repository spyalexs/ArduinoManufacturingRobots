import time

from gui.GUIOutMessage import GUIOutMessage
from getConstants import getCommandLocalizationEffects
from execution.RouteLeg import RouteLeg

from execution.OverSeer import OverSeer

class BotOverSeer(OverSeer):
    # a class to monitor an individual bot's function on the controller end
    
    # for now, the bot overseer will be overriden by direct to bot messages!
    m_directToBotOverride = True

    m_currentLocation = "Node1-A" # the current location of the robot, starts at a default
    m_progressLocation = None
    m_targetLocation = None # the location the bot is currently heading to 
    m_localizing = False # if the robot is currently being localized by the controller

    #commands
    m_patience = .5 # how long to wait before attempting to reissue a command
    m_lastIssue = None
    m_written = False
    m_confirmed = False
    m_messageNumber = 1

    m_commands = [] # list of raw commands to send to bot - ment for manually sending commands
    m_route = [] # list of steps sent to the bot - meant for going through a route
    
    m_status = 0 # the connection status of the bot

    m_itemSlots = None # the number of item slots on the robot
    m_itemsOnBot = [] # a list of the items on the bot

    def __init__(self, macAddress, port, ip_address, queueToBots, queuePacketOut, queueToGUI, connectionType):
        self.m_localizationEffects = getCommandLocalizationEffects()

        super().__init__(macAddress, port, ip_address, queueToBots, queuePacketOut, queueToGUI, connectionType)

        #specify that this is a bot overseer
        self.m_type = "bot"

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
        
    def updateStatus(self, status, UpdateGui:bool = True):
        self.m_status = status

        print("My status is: " + str(status))

        #send status to gui
        self.sendCommandStatusToGui(status)

        #also clear the patience timer so next command is immediate
        self.m_lastIssue = 0

        if(UpdateGui):
            #update the connection on the gui
            self.sendConnectionStatusToGui()

    def issueCommand(self, command):
        #if no command sequence already being ran, add a sequence to be ran

        #special abort case
        if(command == 255):
            #the current sequence needs to be aborted 
            self.sendMessageToOverseen(self.m_port + "$commandIssue$255")
            
            #note that this breaks the localization tracking
            self.m_localizing = False
            
            self.sendLocalizationStatusToGui()

            self.updateStatus(0, False)

            return

        if len(self.m_commands and self.m_route) == 0:
            self.m_commands = [RouteLeg(command, None, None)]

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
        #run super cycle
        previousConnectionState = self.m_connected

        super().cycle()

        #if connection status has changed
        if(not self.m_connected == previousConnectionState):
            #update information on the gui
            self.updateStatus(0)

        if not (self.m_connected):
            #if disconnected, clear all commands
            self.m_commands = []
                                    
            self.m_written = False
            self.m_confirmed = False
        
        #check the bots status
        if(self.m_status == 255):
            #if a command was aborted
            self.m_commands = [] # clear all commands

            #set status back to ready 
            self.updateStatus(0, UpdateGui=False)

            self.m_written = False

        elif(self.m_status == 254):
            #if the command is finished
           
            #ensure location is updated every time a command is finished
            if(not self.m_targetLocation == None):
                self.setLocation(self.m_targetLocation)
                
                #set status back to ready
                self.updateStatus(0, UpdateGui=False)

        elif (self.m_status == 0):
            if(len(self.m_commands) > 0):
                #if a command needs written to bot

                if(not self.m_written):
                    self.m_written = True # mark so the system knows if the systemhas already written a command
                    self.m_confirmed = False #command has not yet been confirmed

                    #write command to bot
                    self.sendMessageToOverseen(self.m_port + "$commandIssue$" + str(self.m_commands[0].m_step))

                    #note the target location for localization purposes
                    self.m_targetLocation = self.m_commands[0].m_endingLocation
                    self.m_progressLocation = self.m_commands[0].m_progressLocation
            
        elif(self.m_status == 253 and self.m_written == True):
            # if the status is 253, the robot is waiting for confirmation to run the command - essential for ensure commands do not get issued twice
            # sorta - kinda a handshake

            if(not self.m_confirmed):
                self.m_confirmed = True
                self.m_written = False

                self.m_commands.pop(0)


                #issue confirmation
                self.sendMessageToOverseen(self.m_port + "$commandIssue$" + str(253))
                
        elif(self.m_status == 2):
            #set the robot's location to the in progress point
            self.setLocation(self.m_progressLocation, Clearing=False)


    def sendCommandStatusToGui(self, status):
        #put a gui message in the GUI queue
        message = GUIOutMessage(self.m_port, "commandStatus", status)
        self.m_queueGui.put(message)

    def sendLocalizationStatusToGui(self):
        #put a localization status message in the the GUI queue
        message = GUIOutMessage(self.m_port, "localizationStatus", int(self.m_localizing))
        self.m_queueGui.put(message)

    def sendLocationToGui(self):
        #send the gui a message with the robot's location
        message = GUIOutMessage(self.m_port, "locationCurrent", self.m_currentLocation)
        self.m_queueGui.put(message)

    def sendInitialBotStateToGUI(self):
        #send the initial state of the robot to the gui... this is needed because the gui is brought up significantly after the overseers

        #super
        super().sendInitialStateToGUI()

        #location
        self.sendLocationToGui()

        #localization
        self.sendLocalizationStatusToGui()

    def setLocation(self, location, Clearing:bool = True):
        if(location == None):
            #none will be passed from target/progress location from commands
            return

        if(Clearing):
            #clear target location
            self.m_targetLocation = None

            #clear progress location
            self.m_progressLocation = None 

        if(not self.m_currentLocation == location):
            #the current location of the robot
            self.m_currentLocation = location

            #send the gui the robot's location
            self.sendLocationToGui()

        if(self.m_localizing == False):
            #mark the robot as now localizing
            self.m_localizing = True
            
            #tell the gui that the robot is now localizing
            self.sendLocalizationStatusToGui()

    def setNumberOfItemSlots(self, numberOfSlots):
        #if the number of slots has changed
        if(not self.m_itemSlots == numberOfSlots):

            self.m_itemSlots = numberOfSlots

            #send message to bot to set the number of item slots
            self.sendMessageToOverseen(self.m_port + "$SetSlotCount$" + numberOfSlots)

            #clear items on bot
            self.m_itemsOnBot = []
        
    def addItem(self, itemType):
        #check to ensure the bot's item slots have been configured
        if(self.m_itemSlots == None):
            print("The bot's item slots must be configured before adding items: " + self.m_port)
            return
        
        #check to make sure the bot is has space for the item
        if(len(self.m_itemsOnBot) >= int(self.m_itemSlots)):
            print("Cannot add item: " + str(itemType) + ". The bot: " + self.m_port + " is at capacity.")
            return

        #add item to the overseer's list of items on the bot
        self.m_itemsOnBot.append(itemType)

        #add item to bot
        self.sendMessageToOverseen(self.m_port + "$AddItem$" + itemType)

    def removeItem(self, itemType):
        #check to ensure the bot's item slots have been configured
        if(self.m_itemSlots == None):
            print("The bot's item slots must be configured before adding items: " + self.m_port)
            return
        
        #check to make sure the bot is has space for the item
        if not (itemType in self.m_itemsOnBot):
            print("Cannot remove item: " + str(itemType) + ". It is not on bot: " + self.m_port)
            return

        #add item to the overseer's list of items on the bot
        self.m_itemsOnBot.remove(itemType)

        #add item to bot
        self.sendMessageToOverseen(self.m_port + "$RemoveItem$" + itemType)
