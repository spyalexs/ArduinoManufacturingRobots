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

def getCommandLocalizationEffects():
    #returns the dictionary that maps the command name to wether or not the command effects the robot's localization

    # -1 - breaks localization
    # 0 - no effect on localization
    # 1 - effects localization but does not break

    keys = {1 : 0, 
            2 : 1, 
            3 : -1, 
            4 : -1, 
            5 : -1, 
            6 : -1, 
            7 : -1,
            101 : 1, 
            102 : 1, 
            103: 1,
            104: 1,
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

def getPacketLength():
    #return the maximum length of a packet

    MAX_PACKET_LENGTH = 50

    return MAX_PACKET_LENGTH

def getTheme():
    #returns the theme for the GUI

    #potentially implement custom theme
    #GUI_THEME "DarkRed1" #OSU theme but hurts eyes
    GUI_THEME = "BrownBlue"

    return GUI_THEME #better to look at

