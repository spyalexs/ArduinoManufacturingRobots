import PySimpleGUI as sg
import threading
import sys

from gui.monitorGUI import getTheme


def createStartupGui(queueIn, queueOut):
    #this is blocking -- everything should wait until the bridge has started

    #queue should just contain simple strings
    #queue in contains robots that have been connected to the system
    #queue out contains action after close of startup 

    #make theme match
    sg.theme(getTheme())

    #layout for startup menu
    layout = [[sg.Text('Starting System... Connected Robots: '), sg.Text("0", key="connectedCount")],
              [sg.Multiline("", size=(50,10), key="startupOutput", autoscroll=True)],
              [sg.HorizontalSeparator()],
              [sg.Text("Before Proceeding, ensure all robots / stations are in their starting positions!")],
              [sg.Button(button_text="Proceed", key="startupProceed"), sg.Button(button_text="Cancel", key="Cancel")]]
    
    window = sg.Window("Model StartUp", layout, finalize=True)
    window.close_destroys_window = True

    return monitorStartUpGUI(window, queueIn, queueOut)

def monitorStartUpGUI(window, queueIn, queueOut):
    connectedBots = []

    while(True):
        event, values = window.read(timeout=100)

        #handle events
        if event in (sg.WINDOW_CLOSED, "Cancel"):
            #tell system to stop and then close
            queueOut.put("Stop")
            break
        elif event == "startupProceed":
            #tell system to proceed and then proceed
            queueOut.put("Proceed")
            break

        #write robots to output
        while(not queueIn.empty()):
            connection = queueIn.get()
            window["startupOutput"].print(str(connection))
                        
            connectedBots.append(str(connection.split(": ")[0]))

        
    window.close()

    return connectedBots
    
