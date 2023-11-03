import threading
import copy
import time

#this is a hopefully thread safe singleton that holds all robot state data...
class StateMap:
    _lock = threading.Lock()
    _instance = None

    #need to use getters and setters
    #stores (isLocalizing, position, status, claimingIntersectionStatus, updatets) of robots by port
    _map = dict()

    def __new__(cls):
        #when a new instance is being created
        if(cls._instance is None):

            #get a lock and make the instance
            with cls._lock:

                #anther thread could have decided to make one while another threasd gets the lock
                if not cls._instance:
                    cls._instance = super().__new__(cls)

        return cls._instance

    def updateBot(cls, bot, localizing, location, status, claiming):
        #get a lock
        with cls._lock:
            #update entry
            cls._map[bot] = (localizing, location, status, claiming, time.time())

        return
    
    def getData(cls):
        data = None

        #get a lock
        with cls._lock:
            #copy map
            data = copy.deepcopy(cls._map)

        return data



    

