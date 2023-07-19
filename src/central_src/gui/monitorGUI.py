import PySimpleGUI as sg
import threading
from PIL import Image, ImageTk

from getConstants import getCommandKeys, getTheme, getItemInformation
from gui.GUIInMessage import GUIInMessage
from createRoute import route, getLocations
from gui.map_display.displayMap import getBaseMap, drawBots

#keys for the commands availible to be launched - the name of the command corrosponds to the name that will be launched on the robot
commandKeys = getCommandKeys()
#the map with no bots draw on
baseMap = getBaseMap()
#the current bot locations as far as the gui knows
botLocationsGUI = dict()

def make(queueIn, queueOut, robots, stations, killQueue):
    #create the GUI window for the first time

    #apply gui theme
    sg.theme(getTheme())

    #make a frame containing all of the robot subframes
    robotRows = []
    counter = 0
    #divy up robots into groups
    while (counter < len(robots)):
        #number of robots to put into a row
        robotsInRow = 3
        row = []

        #fill row with robots
        innerCounter = 0
        while (counter + innerCounter < len(robots) and innerCounter < robotsInRow):
            row.append(robots[counter + innerCounter])
            innerCounter += 1

        #new row
        robotRows.append(row)
        counter += innerCounter

    robotsFrameLayout = []

    #determine if all the rows are full or if there is space for the station
    rowsFull = False
    if(counter % 3 == 0):
        rowsFull = True
    

    for robotRow in robotRows:
        #each row of robot pannels
        robotRowLayout = []
        for robot in robotRow:
            #get frame for each robot
            frameLayout= getRobotFrameLayout(robot)
            robotRowLayout.append(sg.Frame(robot, frameLayout))

        robotsFrameLayout.append(robotRowLayout)

    #make station frame
    if(len(stations) >= 1):
        #if a station has been connected -- add pannel

        #process item keys
        itemKeys = []
        for key in getItemInformation().keys():
            itemKeys.append(key)


        stationFrame = sg.Frame("Stations", getStationFrame(stations, robots, itemKeys))

        #if there is space in the rows, add the station frame to a not full row
        if not rowsFull:
            robotsFrameLayout[(len(robotsFrameLayout) - 1)].append(stationFrame)
        else:
            #no space, make a new row
            robotsFrameLayout.append([sg.Frame(robot, stationFrame)])

    #get the layout for the map sceen
    mapLayout = [[sg.Image(key="MapImage")]]

    #all the elements on the gui are defined here - similar to XML formating...
    layout = [[sg.Frame("",robotsFrameLayout), sg.Frame("Map", mapLayout)]]

    #links the layout to the window
    window = sg.Window(title="Test", layout=layout, finalize=True)

    mapImage = Image.fromarray(baseMap)
    window["MapImage"].update(data=ImageTk.PhotoImage(mapImage))

    #tell central that the gui is ready to begin
    queueIn.put(GUIInMessage("All", "ready", True, Direct=False))

    #keeps an eye on the gui for event and such as it runs
    monitor(window, queueIn, queueOut, killQueue, robots)

def getRobotFrameLayout(name):
    #do a little processing to format the keys correctly
    commands = []
    for key in commandKeys.keys():
        commands.append(key)

    locations = getLocations() 
    
    #returns the layout for a particular robot frame
    robotFrameLayout = [
        [sg.Text("Command"), sg.Combo(commands, enable_events=False, key=(str(name) + "CommandMenu")), sg.Button("Send", key=(str(name) + "Command"), enable_events=True)],
        [sg.Text("     Status"), sg.ProgressBar(100, size=(13,17), key=(str(name) + "CommandProgress")), sg.Button("Abort", key=(str(name)) + "AbortCommand", enable_events=True)],
        [sg.Text("From:"), sg.Input("", key=(str(name) + "RouteFrom"), size=(10,7)), sg.Text("To:"), sg.Input("", key=(str(name) + "RouteTo"), size=(10,7)), sg.Button("Route", key=(str(name) + "Route"), enable_events=True)],
        [sg.Text("Location"), sg.Combo(locations, enable_events=False, size=(9,7), key=(str(name) + "LocationMenu")), sg.Button("Set", key=(str(name) + "SetLocation"), enable_events=True), sg.Text(" Localizing:"), sg.Radio("","2", key=str(name)+"IsLocalizing", circle_color="red")],
        [sg.Text("Battery Voltage: "), sg.Text("0.00", key=str(name) + "BatteryVoltage"), sg.Text("Connection Status:"), sg.Radio("", "1", key=str(name) + "connectionStatus", circle_color="white")]]
    
    return robotFrameLayout

def getStationFrame(connectedStations, connectedRobots, items):
    #get the station frame layout

    stationFrameLayout = [
        [sg.Text("Station"), sg.Combo(connectedStations, enable_events=False, key="SelectedStation", default_value=connectedStations[0])],
        [sg.Text("Item"), sg.Combo(items, enable_events=False, key="StationItem"), sg.Button("Set Item", key="SetStationItem", enable_events=True)],
        [sg.Text("Transfer", key="Station Transfer"), sg.Combo(["To", "From"], enable_events=False, key="StationTransferDirection"), sg.Combo(connectedRobots, enable_events=False, key="StationTransferTarget"), sg.Button("Transfer", key="TransferStationItem", enable_events=True)],
        [sg.Text("Battery Voltage: "), sg.Text("0.00", key="SelectedStationBatteryVoltage"), sg.Text("Connection Status:"), sg.Radio("", "1", key="SelectedStationconnectionStatus", circle_color="white")]
    ]

    return stationFrameLayout

def monitor(window, queueIn, queueOut, killQueue, robots):
    #monitor the Gui for events

    while True:  
        #sequence that allows program to gracefully exit
        if(not killQueue.empty()):
            break

        #wait for events(UI element interaction) on gui
        # timeout allows for the window to be updated without an event occuring         
        event, values = window.read(timeout=1000)

        if event in (sg.WIN_CLOSED, 'Cancel'):
            #if the cancel button is clicked, close the GUI
            break
        elif (event != "__TIMEOUT__"):
            #if there is an interaction, handle it
            handleEvents(event, values, window, queueIn)

        #update the attributes displayed on the window
        update(window, values, queueOut, robots)
    
def handleEvents(event, values, window, queueIn):
    #handle the gui events

    if("AbortCommand" in event):
        #abort the command currently running
        queueIn.put(GUIInMessage(event.split("AbortCommand")[0], "commandIssue", 255, Direct=False))  
    elif("Command" in event):       
        #publish command to bot
        if(values[event + "Menu"] != ''):

            queueIn.put(GUIInMessage(event.split("Command")[0], "commandIssue", commandKeys[values[event + "Menu"]], Direct=False))   

    if("Route" in event):
        #run a route between two points
        if (not values[event.split("Route")[0] + "RouteFrom"] == "") and (not values[event.split("Route")[0] + "RouteTo"] == ""):

            #create a thread to find the route between the two nodes
            #currently not worried about ending thread as this should not ever enter and infinite loop and ideally would end within a couple seconds
            routingThread = threading.Thread(
                target=runRouting, 
                args=(values[event.split("Route")[0] + "RouteFrom"], values[event.split("Route")[0] + "RouteTo"], event.split("Route")[0], queueIn))
            routingThread.isDaemon=True
            routingThread.run()
        
    
    if("SetLocation" in event):
        #Set the location of a robot
        if(not values[event.split("SetLocation")[0] + "LocationMenu"] == ""):
            queueIn.put(GUIInMessage(event.split("SetLocation")[0], "locationSet", values[event.split("SetLocation")[0] + "LocationMenu"], Direct=False))

    if("SetStationItem" in event):
        #set the item a station transfers
        
        if(not values["StationItem"] == ""):
            queueIn.put(GUIInMessage(values["SelectedStation"], "SetStationItem", values["StationItem"], Direct=False))

    if("TransferStationItem") in event:
        #send or recieve an item from a bot

        if(not values["StationTransferTarget"] == "" and not values["StationTransferDirection"] == ""):
            
            #transfer direction
            if(values["StationTransferDirection"] == "To"):
                #item to bot from station
                queueIn.put(GUIInMessage(values["SelectedStation"], "DispenseStationItem", values["StationTransferTarget"], Direct=False))
            else:
                #item from bot to station
                queueIn.put(GUIInMessage(values["SelectedStation"], "CollectStationItem", values["StationTransferTarget"], Direct=False))




def update(window, values, queueOut, robots):
    # to effectively clear queue - do two stage message scanning

    inputDictionary = dict()
    while(not queueOut.empty()):
        # the queue should be full of GUIOutMessages
        #ensure the GUI is only updated based on the latest messages
        #print(queueOut.qsize())

        message = queueOut.get()
        inputDictionary[message.m_target + "$" + message.m_characteristic] = message.m_value

    mapUpdated = False # if the map has been updated and needs to be redrawn

    for topic in inputDictionary:
        #set the location of the bot

        #get the target robot/station
        target = str(topic).split("$")[0]

        #if target is a station - see if its the selected on
        if(target == values["SelectedStation"]):
            #change the target frame to the selected station window
            target = "SelectedStation"

        if("bat" in topic):

            #update battery voltage display
            window[target + "BatteryVoltage"].update(value=f'{inputDictionary[topic]:0,.2f}')

        if("connectionStatus" in topic):
            #set the robot's connect status

            #link value to color
            #2 - unknown = white
            #1 - connected = green
            #0 - disconnected = red

            color = ""# the color to set the radio to
            match inputDictionary[topic]:
                case 1:
                    color = "green"
                case 0:
                    color = "red"
                case 2:
                    color = "white"
                case _:
                    color = "purple"

            #update connection radio
            window[target + "connectionStatus"].update(circle_color = color)

        if("commandStatus" in topic):
            #set the robot's status on the command progress bar

            match inputDictionary[topic]:
                case 0:
                    window[target+"CommandProgress"].update(current_count=0, bar_color=(None, None))
                case 1:
                    window[target+"CommandProgress"].update(current_count=10, bar_color=(None, None))
                case 253:
                    window[target+"CommandProgress"].update(current_count=20, bar_color=("Yellow", None))
                case 2:
                    window[target+"CommandProgress"].update(current_count=50, bar_color=(None, None))
                case 3:
                    window[target+"CommandProgress"].update(current_count=80, bar_color=(None, None))
                case 254:
                    window[target+"CommandProgress"].update(current_count=100, bar_color=("Green", None))
                case 255:
                    window[target+"CommandProgress"].update(current_count=100, bar_color=("Red", None))
        
        if("localizationStatus" in topic):
            #set the status of the localization radio on the displa

            match inputDictionary[topic]:
                case 0:
                    window[target + "IsLocalizing"].update(circle_color="red")

                    #ensure the GUI does not draw the bot location if the bot is not be localized
                    botLocationsGUI[target] = ("Unlocalized")
                case 1:
                    window[target + "IsLocalizing"].update(circle_color="green")
            
            #mark the map to be updated
            mapUpdated = True

        if("locationCurrent" in topic):
            #set the robot's location
            botLocationsGUI[target] = inputDictionary[topic]
            
            #update the origin for the route setter for the particular bot's frame
            window[target + "RouteFrom"].update(value=inputDictionary[topic])
                    
            #mark the map to be updated
            mapUpdated = True

    if(mapUpdated):
        #if the map needs redrawn
        window["MapImage"].update(data=ImageTk.PhotoImage(Image.fromarray(drawBots(baseMap.copy(), botLocationsGUI))))


def runRouting(startingNodeName, endingNodeName, targetBot, queue):
    #route and add route to target - run this in a seperate thread to prevent blocking in gui thread
    commands = route(startingNodeName, endingNodeName)

    if not commands == None:
        #ensure their are commands - fairly easy to make an impossible route

        #send commands to controlled to be processed
        queue.put(GUIInMessage(targetBot, "commandSequence", commands, Direct=False))

        #send message to bot telling it its destination
        queue.put(GUIInMessage(targetBot, "destination", commands[len(commands) - 1].m_endingLocation, Direct=True))
    else:
        print("Failed to route between: " + startingNodeName + " and " + endingNodeName)

