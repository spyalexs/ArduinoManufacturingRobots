import PySimpleGUI as sg
import datetime
from gui.GUIInMessage import GUIInMessage

UPDATE_INTERVAL = 250

def make(queueIn):
    
    theme = sg.theme(getTheme())

    layout = [  [sg.Text('Robot 1')],
                [sg.Text("MindControl"), sg.Checkbox('Enable', enable_events=True, default=False,  key="EnableMindControl")],
                [sg.Text("     Built In LED"), sg.Checkbox('Turn On', 0, enable_events=True, key="TurnOnBuiltInLED")],
                [sg.OK(), sg.Cancel()]]

    window = sg.Window(title="Test", layout=layout, finalize=True)
    #let the gui runs
    monitor(window, queueIn)

def monitor(window, queueIn):
    #monitor the Gui for events

    while True:             
        event, values = window.read(timeout=100)

        if event in (sg.WIN_CLOSED, 'Cancel'):
            break
        elif (event != "__TIMEOUT__"):
            handleEvents(event, values, window, queueIn)
            
        update()

    
def handleEvents(event, values, window, queueIn):
    #handle the gui events
    if(event == "EnableMindControl"):
        if values["EnableMindControl"] == True:
            queueIn.put(GUIInMessage("bot1", "mindControl", "1"))
            window["TurnOnBuiltInLED"].update(disabled=False)
        if values["EnableMindControl"] == False:
            queueIn.put(GUIInMessage("bot1", "mindControl", "0"))
            window["TurnOnBuiltInLED"].update(disabled=True)

    if(event == "TurnOnBuiltInLED"):
        if values["TurnOnBuiltInLED"] == True:
            print("Sending On")
            queueIn.put(GUIInMessage("bot1", "BUILTIN_LED", "1"))
        if values ["TurnOnBuiltInLED"] == False:
            print("Sending Off")
            queueIn.put(GUIInMessage("bot1", "BUILTIN_LED", "0"))
    

def update():
    if False:
        print("Updating GUI")
    
    

def getTheme():
    #potentially implement custom theme

    #return "DarkRed1" #OSU theme but hurts eyes
    return "BrownBlue" #better to look at
