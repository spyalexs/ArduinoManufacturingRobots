def getCommandKeys():
    #returns the dictionary that maps the command name to the keys

    keys = {"Test" : 1, 
            "FollowLineTillMarker" : 2, 
            "FollowLineOnMarker" : 3, 
            "TravelStraight" : 4, 
            "TurnRight" : 5, 
            "TurnLeft" : 6, 
            "TurnU" : 7,
            "IntersectionRight" : 101, 
            "IntersectionStraight" : 102, 
            "IntersectionLeft": 103,
            "IntersectionU": 104,
            }

    return keys

def getConnectionPorts():
    #ports for communication to be assigneed
        #this limit is 20(could be higher) -- need a router for more than 8(hardware)


    LOWER_COM_PORT = 5010
    UPPER_COM_PORT = 5029

    return (LOWER_COM_PORT, UPPER_COM_PORT)

def getListeningPort():
    #return the port the bot is listening on

    BOT_LISTENING_PORT = 5005
    
    return BOT_LISTENING_PORT