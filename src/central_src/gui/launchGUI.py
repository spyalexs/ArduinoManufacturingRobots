import PySimpleGUI as sg
import threading
import time
import queue

from gui.monitorGUI import make
from gui.startupGUI import createStartupGui

def launchGUI(queueIn, queueOut, solutionOut):
    #start the thread the contains the GUI - only one tk thread so both will happen here
    #depricated -  this doesn't give pysimple gui the main thread
    #   use launch script instead

    killQueue = queue.Queue()

    monitorGUIThread = threading.Thread(target=runGUI, args=(queueIn, queueOut, killQueue, solutionOut)) 
    monitorGUIThread.daemon = True #gui closes with central program
    monitorGUIThread.start()
    
    #return PID of thread
    return killQueue

def runGUI(queueIn, queueOut, killQueue, solutionOut):
    print("Starting Startup Python GUI")
    connectedBots, connectedStations = createStartupGui(queueOut, queueIn, killQueue)

    print("Starting Runtime Python GUI.")
    make(queueIn, queueOut, solutionOut, connectedBots, connectedStations, killQueue)
