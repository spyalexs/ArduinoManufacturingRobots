import PySimpleGUI as sg
from random import random
from math import floor
import socket
from time import time
import sys
import os

#insert path to grab constants
sys.path.insert(0, os.path.join(os.path.dirname(os.path.realpath(__file__)), "../.."))

from src.central_src.getConstants import getCentralHostName, getRouterHostName,getHotSpotHostname

BUFFER_LENGTH = 50

def launch():
    #characteristics that the sim can publish
    characteristics = ["bat", "commandStatus", "connectToCentral"]

    #the sim layout
    layout = [[sg.Text("Characteristic:"), sg.Combo(characteristics, key="characteristic")],
              [sg.Text("Value:"), sg.Input(size=(10, 10), key="value"), sg.Button("Send", key="sendMessage", enable_events=True)],
              [sg.Multiline(autoscroll=True, key="output", size=(32,10))]]
    
    #the sim window
    window = sg.Window("Connection Sim", layout, finalize=True)

    monitor(window)

def monitor(window):
    #the port to send messages to before connection
    sendPort = 5006
    
    #port to listen to
    recievePort = 5005

    #port to send non-connection messages to
    targetPort = None

    #to find server, first try hotspot, then external network, then fail
    try:
        #get hotspot hostname
        LOCAL_IP = socket.gethostbyname(getHotSpotHostname())
        if(LOCAL_IP):
            print("Accepting connections over local hotspot!")
    except:
        try:
            LOCAL_IP = socket.gethostbyname(getCentralHostName())
        
            #assume the ip of the router
            ipBytes = LOCAL_IP.split(".")
            ROUTER_IP = str(ipBytes[0]) + "." + str(ipBytes[1]) + "." + str(ipBytes[2]) + ".1"

            ROUTER_NAME = socket.gethostbyaddr(ROUTER_IP)[0]
            if(ROUTER_NAME == getRouterHostName()):
                print("Accepting Connections over switch: " + ROUTER_NAME)
            else:
                print("Invalid switch name: " + str(ROUTER_NAME) + ". Should be: " + str(getRouterHostName()))
                quit()

        except socket.gaierror:
            print("Could not find LOCALIP, is the environment set up?")
            quit()

    #set up send and recieve sockets
    recevingSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    recevingSocket.bind((LOCAL_IP, recievePort))
    recevingSocket.settimeout(.005)

    sendingSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    mac = generateRandomMAC()

    #last battery -- last time the sim published its battery -- do every second to maintain connection
    lastBattery = time()
    
    while(True):
        #read in events from window
        event, values = window.read(timeout=100)

        if(event == sg.WIN_CLOSED):
            #end program if the window is closed
            return
        elif(event == "sendMessage"):

            if(values["characteristic"] == "connectToCentral"):
                connectionMessage =  bytes("Im a bot! MAC:" + mac + "$$$", "utf-8")
                sendingSocket.sendto(connectionMessage, (LOCAL_IP, sendPort))

            elif(not targetPort == None):
                #ensure the target port has been assigned first

                if(values["characteristic"] == "bat"):
                    message = bytes("bat$" + f"{(random() * 7):0,.2f}" + "$$$", "utf-8")
                    sendingSocket.sendto(message, (LOCAL_IP, targetPort))

                    #update the last battery timer
                    lastBattery = time()

                if(values["characteristic"] == "commandStatus"):
                    if(not values["value"] == ""):
                        message = bytes("commandStatus$" + values["value"] + "$$$", "utf-8")
                        sendingSocket.sendto(message, (LOCAL_IP, targetPort))

            else:
                print("Cannot send message, no target port. Must send connection message first!")

        
        #read messages from recieve port
        recieveingFromCentral = True
        while(recieveingFromCentral):
            try:
                message, _ = recevingSocket.recvfrom(BUFFER_LENGTH)

                if(not str(message) == ""):
                    #get usable part of message
                    messageString = (str(message).split("$$$")[0]).split("b'")[1]

                    #break down into sub messages
                    subMessages = messageString.split("$$")

                    for subMessage in subMessages:
                        #check to see if the message is a connection message
                        if("Connection Established on:" in subMessage):
                            #if connection message, assign new talking port
                            connectionMessageParts = subMessage.split(":")
                            
                            try:
                                targetPort = int(connectionMessageParts[1])

                                #switch over the recieving port
                                recevingSocket.close()
                                recevingSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                                recevingSocket.bind((LOCAL_IP, targetPort + 100))
                                recevingSocket.settimeout(.005)

                                print("It is now safe to open another connection sim!")

                            except (IndexError, TypeError):
                                print("Could not assign target port, incorrect connection message.")

                        window["output"].print(subMessage)

            except TimeoutError:
                recieveingFromCentral = False

        if(lastBattery + 1 < time() and not targetPort == None):
            #auto send battery messages to keep connection happy
            message = bytes("bat$" + f"{(random() * 7):0,.2f}" + "$$$", "utf-8")
            sendingSocket.sendto(message, (LOCAL_IP, targetPort))

            #update the last battery timer
            lastBattery = time()


        
def generateRandomMAC():
    #create a random mac address to send to central
    
    counter = 0
    mac = ""
    while counter < 12:
        number = int(floor(random() * 16))
        character = ""

        if(number > 9):
            match number:
                case 10:
                    character = "A"
                case 11:
                    character = "B"
                case 12:
                    character = "C"
                case 13:
                    character = "D"
                case 14:
                    character = "E"
                case 15:
                    character = "F"
        else:
            character = str(number)

        mac = mac + character

        #add colcons to mac in correct places
        if(counter % 2 == 1):
            if(not counter == 11):
                mac = mac + ":"

        counter += 1

    return mac



if __name__ == "__main__":
    print(launch())