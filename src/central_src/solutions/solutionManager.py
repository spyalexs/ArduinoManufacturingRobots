import os
import sys
from queue import Queue
from threading import Thread
from time import time

from solutions.configurationManger  import processConfigurationCommand, validateCommand, getBotCount

#add top level directory to path
sys.path.insert(0, os.path.join(os.path.dirname(os.path.realpath(__file__)), ".."))

from gui.GUIInMessage import GUIInMessage

solutionFilesPath = os.path.join(os.path.dirname(os.path.realpath(__file__)), "solutions")

def getSolutionsFilesList():
    #get a list of valid solution files to display on gui

    #get all files in solution files directory
    files = os.listdir(solutionFilesPath)

    #filter so only .ex is left
    solutionsFiles = []
    for file in files:
        if ".ex" in file:
            solutionsFiles.append(file)

    return solutionsFiles

def startSolution(fileName: str, GuiInQueue: Queue, SolutionQueue:Queue, OverseerData, killQueue:Queue):
    #start a new thread to handle a solution
    print("Preparing to start a new solution: " + str(fileName))

    #clear solution queue before running
    while( not SolutionQueue.empty()):
        SolutionQueue.get()

    solutionThread = Thread(target=runSolution, args=(fileName, GuiInQueue, SolutionQueue, OverseerData, killQueue))
    solutionThread.isDaemon = True
    solutionThread.start()

def runSolution(fileName: str, GuiInQueue: Queue, SolutionQueue:Queue, OverseerData, killQueue:Queue):
    #run through a solution script

    #load in solution file
    try:
        solutionFile = open(os.path.join(solutionFilesPath, fileName), "r")
    except FileNotFoundError:
        print("Cannot open solutions file: " + fileName + ". Failing!")
        return

    #process the file into commands
    solutionsFileContents = solutionFile.read()
    solutionsFileLines = solutionsFileContents.split("\n")

    #filter to valid commands
    solutionsCommands = []
    for line in solutionsFileLines:
        #if the first line starts with a valid command identifier
        if(line[:1] == "S" or line[:1] == "C" or line[:1] == "B"):

            #split line by space to get parameters and command
            solutionsCommands.append(line.split(" "))

        elif(not (line[:1] == "" or line[:1] == "#")):
            #if line is not command, comment or empty space
            print("Invalid command: " + line + ". Not running command but continuing!")
    
    #the status/location of all bots
    statuses = dict()

    #wait for all bots to echo their status
    statusRequestStart = time()
    botCount = getBotCount(OverseerData)
    uniqueStatusesRecieved = 0

    #request bot statuses
    statusRequestMessage = GUIInMessage("5010", "CollectStatuses", "1234", Direct=False)
    GuiInQueue.put(statusRequestMessage)

    while(uniqueStatusesRecieved < botCount):

        if(time() > statusRequestStart + 30):
            #timeout
            print("Failed To recieve statuses from all bots in a reasonable time. Check connections. Dumping Statuses! Failing!")

            #stop the entire controller...
            killQueue.put("Kill")
            quit()

        if(not SolutionQueue.empty()):
            #a status has been sent

            status = SolutionQueue.get()

            if(not status[1] == 0):
                #this is a problem
                print("Please ensure no robots are running commands before running a solution! Failing!")
                return
            
            #check if status is unique or not
            if(status[0] in statuses.keys()):
                #non unique status
                statuses[status[0]] = (status[0], status[1])
            else:
                #unique status
                statuses[status[0]] = (status[0], status[1])
                uniqueStatusesRecieved += 1

    print("All bots are ready to begin solution!")

    #make a list of bots that haven't finished all their commands
    unfinishedBots = statuses.keys()
    solutionStartTime = time()

    #main solution loop - run until all bots are finished
    while(len(unfinishedBots) > 0 and killQueue.empty()):
        
        #updates statuses
        while(not SolutionQueue.empty()):
            #update the status
            statusUpdate = SolutionQueue.get()

            #if the status is to stop the solution
            if(statusUpdate == str("Stop Solution")):
                quit()

            statuses[statusUpdate[0]] = (statusUpdate[0], statusUpdate[1])

        #if a command for the bot was found
        foundCommand = False

        #issues commands / check for finished bots
        for bot in unfinishedBots:

            for command in solutionsCommands:
                 
                #if the bot is ready for a new command
                if(statuses[bot][1] == 0):

                    if(command[0] == "B100"):
                        #if the command is a routeing command

                        if(validateCommand(command, OverseerData, 4, True)):
                            #if the command is valid

                            #if the command is for the bot
                            if(command[1] == bot):
                                foundCommand = True

                                if(solutionStartTime + float(command[2]) < time()):
                                    #if the command is supposed to start now
                                    routeRequest = GUIInMessage(bot, "Route", command[3], Direct=False)
                                    GuiInQueue.put(routeRequest)

                                    #mark that the bot is going to complete the command
                                    command[0] = "Complete"

                                    #mark that the bot has been issued a command
                                    statuses[bot] = (bot, 1)

                                else:
                                    #command is good - just need to wait to run it
                                    break

                        else:
                            print("Invalid routing command: " + str(command[0]) + " " + str(command[1]) + ". Skipping!")

                            command[0] = "Skipped"

                    elif(command[0] == "S100"):
                        #if the commmand is a transfer to bot command

                        if(validateCommand(command, 4, True)):
                            #if the command is valid

                            #if the command is for the bot
                            if(command[1] == bot):    
                                if(solutionStartTime + float(command[2]) < time()):
                                    #if the command is supposed to start now
                                    print("Issuing")

                                    #mark that the bot is going to complete the command
                                    command[0] = "Complete"

                                    #mark that the bot has been issued a command
                                    statuses[bot] = (bot, 1)
                                else:
                                    #command is good - just need to wait to run it
                                    break
                        else:
                            print("Invalid routing command: " + str(command[0]) + " " + str(command[1]) + ". Skipping!")

                            command[0] = "Skipped"

                    elif(command[0] == "S101"):
                        #if the command is a transfer from bot command

                        if(validateCommand(command, 4, True)):
                            #if the command is valid

                            #if the command is for the bot
                            if(command[1] == bot):
                                if(solutionStartTime + float(command[2]) < time()):
                                    #if the command is supposed to start now
                                    print("Issuing")

                                    #mark that the bot is going to complete the command
                                    command[0] = "Complete"

                                    #mark that the bot has been issued a command
                                    statuses[bot] = (bot, 1)
                                else:
                                    #command is good - just need to wait to run it
                                    break
                        else:
                            print("Invalid routing command: " + str(command[0]) + " " + str(command[1]) + ". Skipping!")

                            command[0] = "Skipped"
                    elif(not (command[0] == "Skipped" or command[0] == "Complete")):
                        #send command to configuration manager
                        processConfigurationCommand(command, GuiInQueue, OverseerData)



