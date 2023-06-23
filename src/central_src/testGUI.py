from gui.monitorGUI import make
from gui.startupGUI import launchStartupGUI
import queue
import sys
import time

args = sys.argv

if __name__ == "__main__":
    if(len(args) != 2):
        print("Please pass 1 argument, the gui window to test!")

    match (args[1]):
        case "main":
            #create dummy window to test
            emptyQueueIn = queue.Queue()
            emptyQueueOut = queue.Queue()

            make(emptyQueueIn, emptyQueueOut)

        case "startup":
            emptyQueueIn = queue.Queue()
            emptyQueueOut = queue.Queue()

            launchStartupGUI(emptyQueueIn, emptyQueueOut)

            time.sleep(3.0)

            emptyQueueIn.put("Robot1: 32:34:@3:#$2:@3")

            while(emptyQueueOut.empty()):
                time.sleep(.05)

            print(emptyQueueOut.get())
                



