import os
import sys
from threading import Thread
from queue import Queue

#add path to central folder so gui module can be used
pathToCentral = os.path.join(os.path.dirname(os.path.realpath(__file__)), "..")
sys.path.insert(0, pathToCentral)

from gui.GUIInMessage import GUIInMessage

#define the path the configurations file
pathToConfigurations = os.path.join(os.path.dirname(os.path.realpath(__file__)), "configurations")

def getConfigurationFilesList():
    #returns a list of all configuration files

    files = os.listdir(pathToConfigurations) 

    #filter to only configuration files based on extention
    configurationFiles = []
    for file in files:
        if ".cfg" in file:
            configurationFiles.append(file)

    return configurationFiles

def startConfiguring(filename: str, GUIOutQueue:Queue, overseersData):
    #start a new thread to launch the configuration
    configThread = Thread(target=runConfiguration, args=(filename, GUIOutQueue, overseersData))
    configThread.daemon = True
    configThread.run()

def runConfiguration(fileName: str, queue: Queue, overseersData):
    #this should run in another thread - blocks for a while

    #open the configuration file
    configurationFile = None
    try:
        configurationFile = open(os.path.join(pathToConfigurations, fileName), "r")
    except FileNotFoundError:
        print("Could not open configuration file: " + fileName)
        return
    
    #break file into commands
    configurationFileContents = configurationFile.read()
    configurationLines = configurationFileContents.split("\n")

    #find commands
    configurationCommands = []
    for line in configurationLines:

        #if the first line starts with a valid command identifier
        if(line[:1] == "S" or line[:1] == "C" or line[:1] == "B"):

            #split line by space to get parameters and command
            configurationCommands.append(line.split(" "))

        elif(not (line[:1] == "" or line[:1] == "#")):
            #if line is not command, comment or empty space
            print("Invalid command: " + line + ". Not running command but continuing!")

    #harness gui messages to send commands to central
    for command in configurationCommands:
        processConfigurationCommand(command, queue, overseersData)
        

    print("Configuration Complete!")

def processConfigurationCommand(command, queue, overseersData):
    #match the command to action
    match(command[0]):
            case "C1":
                #verify bot count
                if(validateCommand(command, overseersData, 2)):
                    if(getBotCount(overseersData) == int(command[1])):
                       print("Bot count verified!")
                    else:
                       print("Configuration failed, incorrect number of bots connected! There should be " + command[1] + "but there are " + str(getBotCount(overseersData)) + ". Failing")
                       return

            case "C2":
                #verify station count
                if(validateCommand(command, overseersData, 2)):
                    if(getStationCount(overseersData) == int(command[1])):
                       print("Station count verified!")
                    else:
                       print("Configuration failed, incorrect number of stations connected! There should be " + command[1] + "but there are " + str(getBotCount(overseersData)) + ". Failing")
                       return

            case "S1":
                #configure station item
                if(validateCommand(command, overseersData, 3) and checkTarget(overseersData, command[1])):
                    queue.put(GUIInMessage(command[1], "SetStationItem", command[2], Direct=False))

            case "B1":
                #configure bot slots
                if(validateCommand(command, overseersData, 3) and checkTarget(overseersData, command[1])):
                    queue.put(GUIInMessage(command[1], "SetItemCapacity", command[2], Direct=False)) 

            case "B2":
                #configure bot location
                if(validateCommand(command, overseersData, 3) and checkTarget(overseersData, command[1])):
                    queue.put(GUIInMessage(command[1], "locationSet", command[2], Direct=False))

            case "B3":
                #give bot item
                if(validateCommand(command, overseersData, 3) and checkTarget(overseersData, command[1])):
                    queue.put(GUIInMessage(command[1], "AddItem", command[2], Direct=False))            
            
            case "B4":
                #remove bot item
                if(validateCommand(command, overseersData, 3) and checkTarget(overseersData, command[1])):
                    queue.put(GUIInMessage(command[1], "RemoveItem", command[2], Direct=False))


            case _:
                print("Not ready for command: " + command)

def validateCommand(command, overseerData, length:int, includesTarget=False,):
    #check length of command to ensure paramters are correct
    
    if (len(command) == length):
        #check to ensure the target exists
        if(includesTarget == True):
            if(checkTarget(overseerData, command[1]) == True):
                return True
            else:
                print("Cannot find target for command: " + str(command[0]) + " " + str(command[1]) + ". Skipping!")
                return False
        else:
            #no specified target
            return True

    else:
        if(len(command) == length):
            print("Not enough parameters in command: " + str(command[0]) + ". Skipping!")

            return False
        else:
            print("Too many parameters in command: " + str(command[0]) + ". Skipping!")

            return False
        
def getBotCount(overseerData):
    #return how many bots are connected - at least how many have been connected
    count = 0

    for overseer in overseerData.keys():
        if(overseerData[overseer] == "bot"):
            #if the overseer is a bot overseer
            count += 1

    return count

def getStationCount(overseerData):
    #return how many stations are connected - at least how many have been connected
    count = 0

    for overseer in overseerData.keys():
        if(overseerData[overseer] == "station"):
            #if the overseer is a station overseer
            count += 1

    return count

def checkTarget(overseerData, target):
    #check to see if a target exists

    if target in overseerData.keys():

        return True
    else: 
        print("Cannot find target: " + str(target) + ". Skipping!")
    
    return False


if __name__ == "__main__":
    #test parse a configuration file

    commandsQueue = Queue()
    fileName = "testConfig.cfg"
    overseerData = {5010: "bot", 5020: "station"}

    runConfiguration(fileName, commandsQueue, overseerData)

    while(not commandsQueue.empty()):
        print(commandsQueue.get().getDirectString())

