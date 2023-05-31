import PySimpleGUI as sg
import threading

from gui.GUIInMessage import GUIInMessage
from gui.GUIOutMessage import GUIOutMessage

#keys for the commands availible to be launched - the name of the command corrosponds to the name that will be launched on the robot
commandKeys = {"Test":1, "FollowLineTillMarker":2, "FollowLineOnMarker" : 3, "TravelStraight" : 4, "TurnRight" : 5, "TurnLeft" : 6, "IntersectionRight" : 101, "IntersectionStraight" : 102, "IntersectionLeft": 103}

def make(queueIn, queueOut):
    #create the GUI window for the first time
    
    #apply gui theme
    sg.theme(getTheme())

    #do a little processing to format the keys correctly
    commands = []
    for key in commandKeys.keys():
        commands.append(key)

    #all the elements on the gui are defined here - similar to XML formating...
    layout = [  [sg.Text('Robot 1')],
                [sg.Text("Command"), sg.Combo(commands, enable_events=False, key="CommandMenu"), sg.Button("Send", key="Command", enable_events=True)],
                [sg.Text("     Status"), sg.ProgressBar(100, size=(13,17), key="CommandProgress"), sg.Button("Abort", key="AbortCommand", enable_events=True)],
                [sg.Text("MindControl"), sg.Checkbox('Enable', enable_events=True, default=False,  key="EnableMindControl")],
                [sg.Text("     Built In LED"), sg.Checkbox('Turn On', 0, enable_events=True, key="TurnOnBuiltInLED", disabled=True)],
                [sg.Text("     Motor 1"), sg.Slider(key="M1Slider", orientation='h', range=(0,100), default_value=0, disabled=True)],
                [sg.Text("     Motor 2"), sg.Slider(key="M2Slider", orientation='h', range=(0,100), default_value=0, disabled=True)],
                [sg.Text("     Update Motors"), sg.Button("Update", key="MotorUpdate", enable_events=True, disabled=True)],
                [sg.Text("     M1 Encoder: 0", key="E1"), sg.Button("Reset", key="M1Reset", enable_events=True, disabled=True)],
                [sg.Text("     M2 Encoder: 0", key="E2"), sg.Button("Reset", key="M2Reset", enable_events=True, disabled=True)]]

    #links the layout to the window
    window = sg.Window(title="Test", layout=layout, finalize=True)
    
    #keeps an eye on the gui for event and such as it runs
    monitor(window, queueIn, queueOut)

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
    if(event == "EnableMindControl"):
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

    if(event == "TurnOnBuiltInLED"):
        #set the built-in LED
        if values["TurnOnBuiltInLED"] == True:
            queueIn.put(GUIInMessage("bot1", "BUILTIN_LED", "1"))
        if values ["TurnOnBuiltInLED"] == False:
            queueIn.put(GUIInMessage("bot1", "BUILTIN_LED", "0"))

    if(event == "MotorUpdate"):
        #publish to motors
        queueIn.put(GUIInMessage("bot1", "M1", values["M1Slider"]))
        queueIn.put(GUIInMessage("bot1", "M2", values["M2Slider"]))

    if(event == "Command"):
        #publish command to bot
        if(values["CommandMenu"] != ''):
            queueIn.put(GUIInMessage("bot1", "commandIssue", commandKeys[values["CommandMenu"]]))   

    if(event == "AbortCommand"):
        #abort the command currently running
        queueIn.put(GUIInMessage("bot1", "commandIssue", 255))   


def update(window, queueOut):
    # to effectively clear queue - do two stage message scanning

    inputDictionary = dict()
    while(not queueOut.empty()):
        # the queue should be full of GUIOutMessages
        #ensure the GUI is only updated based on the latest messages
        print(queueOut.qsize())

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


