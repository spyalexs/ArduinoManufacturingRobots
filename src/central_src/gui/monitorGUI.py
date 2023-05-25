import PySimpleGUI as sg
import threading

from gui.GUIInMessage import GUIInMessage
from gui.GUIOutMessage import GUIOutMessage

UPDATE_INTERVAL = 250

def make(queueIn, queueOut):
    
    theme = sg.theme(getTheme())

    layout = [  [sg.Text('Robot 1')],
                [sg.Text("Command"), sg.Combo(['Follow Line', 'Follow Line Until Intersection'], enable_events=False, key="CommandMenu"), sg.Button("Send", key="Command", enable_events=True)],
                [sg.Text("MindControl"), sg.Checkbox('Enable', enable_events=True, default=False,  key="EnableMindControl")],
                [sg.Text("     Built In LED"), sg.Checkbox('Turn On', 0, enable_events=True, key="TurnOnBuiltInLED", disabled=True)],
                [sg.Text("     Motor 1"), sg.Slider(key="M1Slider", orientation='h', range=(0,100), default_value=0, disabled=True)],
                [sg.Text("     Motor 2"), sg.Slider(key="M2Slider", orientation='h', range=(0,100), default_value=0, disabled=True)],
                [sg.Text("     Update Motors"), sg.Button("Update", key="MotorUpdate", enable_events=True, disabled=True)],
                [sg.Text("     M1 Encoder: 0", key="E1"), sg.Button("Reset", key="M1Reset", enable_events=True, disabled=True)],
                [sg.Text("     M2 Encoder: 0", key="E2"), sg.Button("Reset", key="M2Reset", enable_events=True, disabled=True)]]

    window = sg.Window(title="Test", layout=layout, finalize=True)
    #let the gui runs
    monitor(window, queueIn, queueOut)

def monitor(window, queueIn, queueOut):
    #monitor the Gui for events

    while True:             
        event, values = window.read(timeout=1000)

        if event in (sg.WIN_CLOSED, 'Cancel'):
            break
        elif (event != "__TIMEOUT__"):
            handleEvents(event, values, window, queueIn)
            
        update(window, queueOut)
    
def handleEvents(event, values, window, queueIn):
    #handle the gui events
    if(event == "EnableMindControl"):
        if values["EnableMindControl"] == True:
            queueIn.put(GUIInMessage("bot1", "mindControl", "1"))
            window["TurnOnBuiltInLED"].update(disabled=False)
            window["M1Slider"].update(disabled=False)
            window["M2Slider"].update(disabled=False)
            window["MotorUpdate"].update(disabled=False)
            window["M1Reset"].update(disabled=False)
            window["M2Reset"].update(disabled=False)
        if values["EnableMindControl"] == False:
            queueIn.put(GUIInMessage("bot1", "mindControl", "0"))
            window["TurnOnBuiltInLED"].update(disabled=True)
            window["M1Slider"].update(disabled=True)
            window["M2Slider"].update(disabled=True)
            window["MotorUpdate"].update(disabled=True)
            window["M1Reset"].update(disabled=True)
            window["M2Reset"].update(disabled=True)


    if(event == "TurnOnBuiltInLED"):
        if values["TurnOnBuiltInLED"] == True:
            queueIn.put(GUIInMessage("bot1", "BUILTIN_LED", "1"))
        if values ["TurnOnBuiltInLED"] == False:
            queueIn.put(GUIInMessage("bot1", "BUILTIN_LED", "0"))

    if(event == "MotorUpdate"):
        #publish to motors
        queueIn.put(GUIInMessage("bot1", "M1", values["M1Slider"]))
        queueIn.put(GUIInMessage("bot1", "M2", values["M2Slider"]))

    
    

def update(window, queueOut):
    # to effectively clear queue - do two stage message scanning

    inputDictionary = dict()
    while(not queueOut.empty()):
        # the queue should be full of GUIOutMessages
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


