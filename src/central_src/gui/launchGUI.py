import PySimpleGUI as sg
import threading
import time

from gui.monitorGUI import make

def launchGUI(queueIn):
    print("Starting Python GUI.")

    monitorGUIThread = threading.Thread(target=make, args=(queueIn,)) 
    monitorGUIThread.daemon = True #gui closes with central program
    monitorGUIThread.start()
