import PySimpleGUI as sg
import threading
import time

from gui.monitorGUI import make
from gui.startupGUI import createStartupGui

def launchGUI(queueIn, queueOut):
    #start the thread the contains the GUI - only one tk thread so both will happen here


    monitorGUIThread = threading.Thread(target=runGUI, args=(queueIn, queueOut)) 
    monitorGUIThread.daemon = True #gui closes with central program
    monitorGUIThread.start()

def runGUI(queueIn, queueOut):
    print("Starting Startup Python GUI")
    connectedBots = createStartupGui(queueOut, queueIn)

    print("Starting Runtime Python GUI.")
    make(queueIn, queueOut, connectedBots)
