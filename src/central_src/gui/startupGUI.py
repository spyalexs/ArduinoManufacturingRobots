import PySimpleGUI as sg
import threading
import sys

from getConstants import getTheme


def createStartupGui(queueIn, queueOut, killQueue):
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

    return monitorStartUpGUI(window, queueIn, queueOut, killQueue)

def monitorStartUpGUI(window, queueIn, queueOut, killQueue):
    connectedBots = []

    while(True):
        #sequence that allows program to gracefully exit
        if(not killQueue.empty()):
            break

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
            #make the message more friendly
            connectionInfo = str(connection).split("$")
            if(len(connectionInfo) >= 3):
                connectionString = connectionInfo[1] + ": " + connectionInfo[0] + "  " + connectionInfo[2]

                window["startupOutput"].print(str(connectionString))
                        
                connectedBots.append(str(connectionInfo[1]))

                #update bot counter
                window["connectedCount"].update(value=str(len(connectedBots)))

        
    window.close()

    return connectedBots
    
