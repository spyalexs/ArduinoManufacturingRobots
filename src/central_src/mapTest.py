import StateMap
import threading
import time

def run2():
    i=0
    while(i < 10):
        print(StateMap.StateMap().getData())
        StateMap.StateMap().updateBot("5010", "this", 38294)

        time.sleep(1.0)
        i+=1
    
th2 = threading.Thread(target=run2)
th2.start()

StateMap.StateMap().updateBot("5010", "this", 3894)
print(StateMap.StateMap().getData())

