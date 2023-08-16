from central import runController, subThreadKills
from threading import Thread
from gui.launchGUI import runGUI

from queue import Queue
import signal
import sys

guiKillQueue = Queue()#sends the gui the kill signal

def cleanKill(signum, frame):
    print("Cleanly shutting down!")

    guiKillQueue.put("Kill")

    for threadQueue in subThreadKills:
        #add something to the kill queue so the thread knows its time to end
        threadQueue.put("Kill")

    quit()

if __name__ == "__main__":
    #this should start the controll in place of central.py
    #this script will ensure the "main thread" is passed to pysimplegui

    #reassgin ctrl-c to be a clean kill   
    signal.signal(signal.SIGINT, cleanKill)

    if not sys.platform.startswith('win'):
        #ensure windows is being used
        print("This program was designed to run only on a Windows operating system! Quitting.")
        quit()

    #make the queues
    queueIn = Queue()
    queueOut = Queue()
    queueInGui = Queue()
    queueOutGui = Queue()
    controllerKill = Queue()

    controllerThread = Thread(target=runController, args=(queueIn, queueOut, queueInGui, queueOutGui, controllerKill))
    controllerThread.daemon = True
    controllerThread.start()

    #run the GUI
    runGUI(queueIn, queueOut, guiKillQueue)




