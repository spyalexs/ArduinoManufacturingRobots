import PySimpleGUI as sg
import threading
import time

from gui.monitorGUI import make

def launchGUI(queueIn, queueOut):
    print("Starting Python GUI.")

    monitorGUIThread = threading.Thread(target=make, args=(queueIn, queueOut)) 
    monitorGUIThread.daemon = True #gui closes with central program
    monitorGUIThread.start()
