import PySimpleGUI as sg
import queue

from getConstants import getCommandKeys
from gui.GUIInMessage import GUIInMessage
from gui.GUIOutMessage import GUIOutMessage
from createRoute import route

#keys for the commands availible to be launched - the name of the command corrosponds to the name that will be launched on the robot
commandKeys = getCommandKeys()

def make(queueIn, queueOut, robots):
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

    for robotRow in robotRows:
        #each row of robot pannels
        robotRowLayout = []
        for robot in robotRow:
            #get frame for each robot
            frameLayout= getRobotFrameLayout(robot)
            robotRowLayout.append(sg.Frame(robot, frameLayout))

        robotsFrameLayout.append(robotRowLayout)

    #all the elements on the gui are defined here - similar to XML formating...
    layout = [[sg.Frame("",robotsFrameLayout)]]

    #links the layout to the window
    window = sg.Window(title="Test", layout=layout, finalize=True)
    
    #keeps an eye on the gui for event and such as it runs
    monitor(window, queueIn, queueOut)

def getRobotFrameLayout(name):
    #do a little processing to format the keys correctly
    commands = []
    for key in commandKeys.keys():
        commands.append(key)

    #returns the layout for a particular robot frame
    robotFrameLayout = [
        [sg.Text("Command"), sg.Combo(commands, enable_events=False, key=(str(name) + "CommandMenu")), sg.Button("Send", key=(str(name) + "Command"), enable_events=True)],
        [sg.Text("     Status"), sg.ProgressBar(100, size=(13,17), key=(str(name) + "CommandProgress")), sg.Button("Abort", key=(str(name)) + "AbortCommand", enable_events=True)],
        [sg.Text("From:"), sg.Input("", key=(str(name) + "RouteFrom"), size=(10,7)), sg.Text("To:"), sg.Input("", key=(str(name) + "RouteTo"), size=(10,7)), sg.Button("Route", key=(str(name) + "Route"), enable_events=True)]]
    
    return robotFrameLayout

def monitor(window, queueIn, queueOut):
    #monitor the Gui for events

    while True:  
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
        update(window, queueOut)
    
def handleEvents(event, values, window, queueIn):
    #handle the gui events
    if("EnableMindControl" in event):
        if values["EnableMindControl"] == True:
            # turn off mindcontrol mode
            queueIn.put(GUIInMessage("bot1", "mindControl", "1"))

            window["TurnOnBuiltInLED"].update(disabled=False)
            window["M1Slider"].update(disabled=False)
            window["M2Slider"].update(disabled=False)
            window["MotorUpdate"].update(disabled=False)
            window["M1Reset"].update(disabled=False)
            window["M2Reset"].update(disabled=False)
        if values["EnableMindControl"] == False:
            #turn on mind control mode
            queueIn.put(GUIInMessage("bot1", "mindControl", "0"))

            window["TurnOnBuiltInLED"].update(disabled=True)
            window["M1Slider"].update(disabled=True)
            window["M2Slider"].update(disabled=True)
            window["MotorUpdate"].update(disabled=True)
            window["M1Reset"].update(disabled=True)
            window["M2Reset"].update(disabled=True)

    if("TurnOnBuiltInLED" in event):
        #set the built-in LED
        if values["TurnOnBuiltInLED"] == True:
            queueIn.put(GUIInMessage("bot1", "BUILTIN_LED", "1"))
        if values ["TurnOnBuiltInLED"] == False:
            queueIn.put(GUIInMessage("bot1", "BUILTIN_LED", "0"))

    if("MotorUpdate" in event):
        #publish to motors
        queueIn.put(GUIInMessage("bot1", "M1", values["M1Slider"]))
        queueIn.put(GUIInMessage("bot1", "M2", values["M2Slider"]))

    if("AbortCommand" in event):
        #abort the command currently running
        queueIn.put(GUIInMessage("bot1", "commandIssue", 255))  
    elif("Command" in event):       
        #publish command to bot
        if(values[event + "Menu"] != ''):
            queueIn.put(GUIInMessage("bot1", "commandIssue", commandKeys[values[event + "Menu"]]))   

 
    if("Route" in event):
        #run a route between two points
        if (not values["RouteFrom"] == "") and (not values["RouteTo"] == ""):
            commands = route(values["RouteFrom"], values["RouteTo"])

            if not commands == None:
                #ensure their are commands - fairly easy to make an impossible route

                #send commands to controlled to be processed
                queueIn.put(GUIInMessage("bot1", "commandSequence", commands, Direct=False))


def update(window, queueOut):
    # to effectively clear queue - do two stage message scanning

    inputDictionary = dict()
    while(not queueOut.empty()):
        # the queue should be full of GUIOutMessages
        #ensure the GUI is only updated based on the latest messages
        #print(queueOut.qsize())

        message = queueOut.get()
        inputDictionary[message.m_characteristic] = message.m_value

    for characteristic in inputDictionary:
        if(characteristic == "E1"):
            window["E1"].update(value="     M1 Encoder:" + str(inputDictionary[characteristic]))

        if(characteristic == "E2"):
            window["E2"].update(value="     M1 Encoder:" + str(inputDictionary[characteristic]))

def getTheme():
    #potentially implement custom theme

    #return "DarkRed1" #OSU theme but hurts eyes
    return "BrownBlue" #better to look at

