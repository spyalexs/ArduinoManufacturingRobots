import time
import threading
import os

from gui.GUIOutMessage import GUIOutMessage
from imageStreaming.image2icon import image2icon

#superclass for bot and station overseer

class OverSeer:

    #connection
    m_connectionTimeout = 5 # after five seconds of not hearing from bot, assume connection issues
    m_connected = False

    m_iconPacketLocation = "" #the location of the icon packets

    def __init__(self, macAddress, port, ip_address, queueToBots, queuePacketOut, queueToGUI):
        self.m_port = port
        self.m_mac = macAddress
        self.m_ip = ip_address
        self.m_queue = queueToBots
        self.m_queuePacketOut = queuePacketOut
        self.m_queueGui = queueToGUI

        self.m_lastIssue = 0

        self.m_lastConnection = time.time()

        self.m_iconPacketLocation = os.path.join(os.path.dirname(os.path.realpath(__file__)), "..", "imageStreaming", "packets")

        self.m_type = "generic"
    
    def sendConnectionStatusToGui(self):
        #put a gui message in the GUI queue
        message = GUIOutMessage(self.m_port, "connectionStatus", int(self.m_connected))
        self.m_queueGui.put(message)

    def sendMessageToOverseen(self, message):
        #send a message to the bot
        self.m_queue.put(self.m_ip + ":" + message)
    
    def connectionHeard(self):
        if(not self.m_connected):
            self.m_connected = True

            #tell the gui to update the connection only if it has changed
            self.sendConnectionStatusToGui()

        #log that a connection was heard from the bot
        self.m_lastConnection = time.time()

    def sendPacket(self, value):
        #open the file corrosponding to the packet
        try:
            #read in packet
            packetFile = open(os.path.join(self.m_iconPacketLocation, value + ".bin"), "rb")
            packetBytes = packetFile.read()

            #send packet and recieve address to output queue
            self.m_queuePacketOut.put(((self.m_ip, int(self.m_port)), packetBytes))

        except FileNotFoundError:
            print("Packet: " + str(value) + " is being generated!")

            #split value into icon name, size and packet number
            
            iconGenerationThread = threading.Thread(target=self.generateAndSendPacket, args=(value,))
            iconGenerationThread.daemon = True
            iconGenerationThread.start()
        
    def generateAndSendPacket(self, value):
        #icon hasn't been generated yet, do this now

        valueArray = value.split("_")

        if(len(valueArray) != 3):
            return

        try:
            image2icon(valueArray[0], int(valueArray[1]))

            try:
                #read in packet
                packetFile = open(os.path.join(self.m_iconPacketLocation, value + ".bin"), "rb")
                packetBytes = packetFile.read()

                print("Sending to publisher")

                #send packet and recieve address to output queue
                self.m_queuePacketOut.put(((self.m_ip, int(self.m_port)), packetBytes))

            except FileNotFoundError:
                print("Attempted to generate packet but unexpectedly failed!") 

        except:
            print("Failed to generate: " + valueArray[0] + "!")

    def sendInitialStateToGUI(self):
        #send the initial state of the overseen to the gui... this is needed because the gui is brought up significantly after the overseers
        
        #connection
        self.sendConnectionStatusToGui()

    def cycle(self):
        # cycle the connection status

        if(self.m_lastConnection + self.m_connectionTimeout < time.time()):
            if(self.m_connected):
                self.m_connected = False

                #tell the gui to update the connection only if it has changed
                self.sendConnectionStatusToGui()