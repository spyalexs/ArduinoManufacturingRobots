import PySimpleGUI as sg
from PIL import Image, ImageTk
import xml.etree.ElementTree as ET
import numpy as np
import sys
import os
import math

#add theme to path
sys.path.insert(1, os.path.join(os.path.dirname(os.path.realpath(__file__)), "..", ".."))

from getConstants import getTheme

NODESIZE = 33 #mutliple of 8 + 1 and must be at leatst 25
MAPFILENAME = "map.xml"
MAPSIZE = 600

#create a map of possible bot locations
botDrawingLocations = dict()

#path to bot images
botImagesPath = os.path.join(os.path.dirname(os.path.realpath(__file__)), "botimages")

def parseFile(file):
    mapTree = ET.parse(file)
    return mapTree.getroot()

def openXMLFile():
    #opens the xml file containing the map
    filePath = os.path.join(os.path.dirname(os.path.realpath(__file__)), MAPFILENAME)

    return open(filePath, 'r')

def displayBaseMap(root):
    #get the theme from the other display to ensure they match
    sg.theme(getTheme())

    #set the root loaded from xml
    layout = [[sg.Image(size=(MAPSIZE,MAPSIZE), key="mapImage")]]

    window = sg.Window(title="Map", layout=layout, finalize=True)
    window["mapImage"].update(data=ImageTk.PhotoImage(drawMap(root)))
    window.read()

def getBaseMap():
    #open map xml file
    file = openXMLFile()

    #get the root node for the map
    root = parseFile(file)

    if root:
        #get the image
        baseImage = drawMap(root)

        return baseImage
    
def flipImageY(img):
    #compensates for the top of the image being y = 0
    #TODO not do this ------ 

    imgHeight = len(img)
    flippedImg = np.zeros(shape=(imgHeight, len(img[0]), len(img[0][0])))

    for i,row in enumerate(img):
        flippedImg[imgHeight - i - 1] = row 
    

    return np.uint8(flippedImg)
    

def loadImage():
    #load a test imahe
    img = Image.open(r".\gui\map_display\test.jpg")
    return ImageTk.PhotoImage(img)

def prepPathPlanningXML(root):
    #prep a path planning xml by finding and listing all sub nodes and intra node connections

    #xml data container
    ppData = ET.Element("map")

    intraConnectionCost = 30 # the cost of making an intranode connection
    uturnConnectionCost = 35 # the cost of making a uturn

    #iterate over all subnodes

    for node in root.findall("./node"):
        for subnode in node.findall("./subnode"):
            ppnode = ET.SubElement(ppData, "ppnode")

            #save pp node data
            ppnodeName = subnode.get("name")
            ppNodeNameArray = ppnodeName.split("-")
            ppnode.set("name", ppnodeName)

            #create and save all of the valid intranode connections from the subnode
            if(ppNodeNameArray[1] == 'B' or ppNodeNameArray[1] == 'D' or ppNodeNameArray[1] == 'F' or ppNodeNameArray[1] == 'H'):
                pplink1 = ET.SubElement(ppnode, "pplink")

                if(ppNodeNameArray[1] == 'H'):
                    #uturn
                    pplink1.set("cost", str(uturnConnectionCost))
                else:
                    #all others
                    pplink1.set("cost", str(intraConnectionCost))  

                pplink1.set("intersection", ppNodeNameArray[0])
                pplink1.set("locationX", node.get("locationX"))
                pplink1.set("locationY", node.get("locationY"))   
                pplink1.set("neighbor", ppNodeNameArray[0] + "-A")  


                pplink2 = ET.SubElement(ppnode, "pplink")

                #determine intranode cost based on type of turn
                if(ppNodeNameArray[1] == 'B'):
                    #uturn
                    pplink2.set("cost", str(uturnConnectionCost))
                else:
                    #all others
                    pplink2.set("cost", str(intraConnectionCost))

                pplink2.set("intersection", ppNodeNameArray[0])
                pplink2.set("locationX", node.get("locationX"))
                pplink2.set("locationY", node.get("locationY"))
                pplink2.set("neighbor", ppNodeNameArray[0] + "-C")   


                pplink3 = ET.SubElement(ppnode, "pplink")

                if(ppNodeNameArray[1] == 'D'):
                    #uturn
                    pplink3.set("cost", str(uturnConnectionCost))
                else:
                    #all others
                    pplink3.set("cost", str(intraConnectionCost))

                pplink3.set("intersection", ppNodeNameArray[0])
                pplink3.set("locationX", node.get("locationX"))
                pplink3.set("locationY", node.get("locationY"))
                pplink3.set("neighbor", ppNodeNameArray[0] + "-E")   
          
                pplink4 = ET.SubElement(ppnode, "pplink")

                if(ppNodeNameArray[1] == 'F'):
                    #uturn
                    pplink4.set("cost", str(uturnConnectionCost))
                else:
                    #all others
                    pplink4.set("cost", str(intraConnectionCost))

                pplink4.set("intersection", ppNodeNameArray[0])
                pplink4.set("locationX", node.get("locationX"))
                pplink4.set("locationY", node.get("locationY"))
                pplink4.set("neighbor", ppNodeNameArray[0] + "-G")  

    return ppData

def drawMap(root):
    #this should be only run once per boot of gui

    map = np.zeros(shape=(300,300,4))
    # map[row][column]

    #get the initial path planning data
    ppData = prepPathPlanningXML(root)

    for node in root.findall("./node"):
        #draw in nodes
        name = node.get('name')
        nodeX, nodeY = getNodeLocation(name, root)
        drawNode(nodeX, nodeY, map)

        #add node to bot drawing locations
        botDrawingLocations[name] = (nodeX, nodeY, "Node")

        #draw in connections 
        for subnode in node.findall("./subnode"):
            subnodeName = subnode.get('name')

            #add subnode to bot drawing locations
            subnodeX, subnodeY = getSubNodeLocation(subnodeName, root, getConnectionLineWidth())
            botDrawingLocations[subnodeName] = (subnodeX, subnodeY, getSubnodeDrawingDirection(subnodeName))

            for connection in subnode.findall('./connection'):
                endName = connection.get("destination")

                drawConnection(map, root, subnodeName, endName, ppData)

                #add midpoint on connection to drawing locations
                botDrawingLocations[subnodeName + "to" + endName] = calculateConnectionDrawingPoint(subnodeName, endName, root)

                actionPointNames = []
                counter = 0
                for actionPoint in connection.findall("./action_point"):
                    counter += 1
                    actionPointNames.append("AP" + actionPoint.get("id"))
                    botDrawingLocations["AP" + actionPoint.get("id")] = calculateActionPointDrawingPoint(subnodeName, endName, root, counter, len(connection.findall("./action_point")))

                #make a connection between every action point and the starting node
                
    #write xml generated to file
    ppTree = ET.ElementTree(ppData)
    ET.indent(ppTree, " ")
    ppTree.write("ppData.xml")

    #flip y
    return np.uint8(map)

def drawNode(locationX, locationY,  map):
    # nodes are represented by an 41 by 41 box, centered around location

    #color to draw node as
    nodeColor = [255, 0, 0, 255]

    #size of node
    nodeRadius = (NODESIZE - 1) / 2

    #check bounds
    if not (locationX >= nodeRadius):
        print("Cannot draw node, bounds!")
        return
    if not (locationY >= nodeRadius):
        print("Cannot draw node, bounds!")
        return
    if not (len(map[0]) > locationX + nodeRadius):
        print("Cannot draw node, bounds!")
        return
    if not (len(map) > locationY + nodeRadius):
        print("Cannot draw node, bounds!")
        return
    
    counterX = locationX - int(nodeRadius)
    while(counterX <= locationX + nodeRadius):
        counterY = locationY - int(nodeRadius)

        while(counterY <= locationY + nodeRadius):
            map[counterY][counterX] = nodeColor
            counterY += 1

        counterX += 1

def getConnectionLineWidth():
    #return the width of connection lines
    return (NODESIZE - 1) / 4

def drawConnection(map, root, originSubNodeName, endSubNodeName, ppData):
    #connection line width 
    lineWidth = getConnectionLineWidth()

    #color to draw connections
    connectionColor = [0,0,0, 255]
    actionPointColor = [0,255,255, 255]

    #check to ensure the node path is not between two subnodes of the same node
    if(originSubNodeName.split("-")[0] == endSubNodeName.split("-")[0]):
        print("no intra subnode connections")
        return

    #subnode 1 location
    subnodeOriginX, subnodeOriginY = getSubNodeLocation(originSubNodeName, root, lineWidth)

    #subnode 2 location
    subnodeEndX, subnodeEndY = getSubNodeLocation(endSubNodeName, root, lineWidth) 

    actionPointIDs = checkForActionPoints(root, originSubNodeName)
    actionPointCount = len(actionPointIDs)


    if(subnodeOriginX == subnodeEndX):
        #vertically alginded nodes
        xmin = subnodeOriginX - lineWidth / 2
        xmax = subnodeOriginX + lineWidth / 2

        ymin = min(subnodeOriginY, subnodeEndY) - lineWidth / 2
        ymax = max(subnodeOriginY, subnodeEndY) + lineWidth / 2

        counterX = int(xmin)
        while(counterX <= xmax):
            counterY = int(ymin)

            while(counterY <= ymax):
                map[counterY][counterX] = connectionColor
                counterY += 1

            counterX += 1

        #distance betweem the two ends
        connectionLength = abs(subnodeEndY - subnodeOriginY)

        # add connection to path planning data
        #find the subnode to add the connection to 
        ppOriginSubnode = None
        for ppNode in ppData.findall("./ppnode"):
            if(ppNode.get("name") == originSubNodeName):
                ppOriginSubnode = ppNode

                #add pplink to subnode
                pplink = ET.SubElement(ppNode, "pplink")
                pplink.set("cost", str(connectionLength))
                pplink.set("locationX", str(subnodeOriginX))
                pplink.set("locationY", str(round((subnodeOriginY + subnodeEndY) / 2)))   
                pplink.set("intersection", "None")
                pplink.set("neighbor", endSubNodeName)
        
        if not ppOriginSubnode:
            print("Cannot find path planning subnode - something is up!")
            return

        if(actionPointCount == 0):
            #don't try to draw action points if there aren't any
            return

        #draw action points evenly spaced
        #take off half a node size on each end add a quater of a node size to each end of action point as a buffer
        spacing, emptySpace = calculateActionPointSpacing(connectionLength, actionPointCount)

        #determine which side to draw action points
        if(subnodeOriginY < subnodeEndY):
            #connection runs bottom to top - draw on x- side
            counter = 0
            while(counter < actionPointCount):
                #draw each action point
                counterX = math.floor((subnodeOriginX - (lineWidth * 1.5) + 2))

                while(counterX < math.floor(subnodeOriginX - (lineWidth * 0.5) - 2)):
                    counterY = math.floor(counter * spacing + subnodeOriginY + (NODESIZE - 1) / 2 + emptySpace)

                    while(counterY < (counter + 1) * spacing + subnodeOriginY + (NODESIZE - 1) / 2 - emptySpace):
                        map[counterY][counterX] = actionPointColor
    
                        counterY +=1

                    counterX +=1

                #add action point as a node
                ppActionNode = ET.SubElement(ppData, "ppnode")
                ppActionNode.set("name", "AP" + actionPointIDs[counter])

                #add link from action point to end node
                pplink = ET.SubElement(ppActionNode, "pplink")
                pplink.set("cost", str((actionPointCount - counter - .5) * spacing + (NODESIZE / 2) + 1))
                pplink.set("locationX", str(subnodeOriginX))
                pplink.set("locationY", str(round(subnodeOriginY + (NODESIZE - 1) / 2 + (counter + .75) * spacing)))
                pplink.set("neighbor", endSubNodeName)
                pplink.set("intersection", "None")


                #add links from action point to action point
                counter2 = counter + 1
                while(counter2 < actionPointCount):
                    pplink3 = ET.SubElement(ppActionNode, "pplink")
                    pplink3.set("cost", str((counter2 - counter) * spacing + 1))
                    pplink3.set("locationX", str(subnodeOriginX))
                    pplink3.set("locationY", str(round(subnodeOriginY + (NODESIZE - 1) / 2 + (counter2 + .25) * spacing)))
                    pplink3.set("neighbor", "AP" + actionPointIDs[counter2])
                    pplink3.set("intersection", "None")

                    counter2 +=1

                #add link from origin to actionPoint
                pplink2 = ET.SubElement(ppOriginSubnode, "pplink")
                pplink2.set("cost", str((counter + .5) * spacing + (NODESIZE / 2) + 1))
                pplink2.set("locationX", str(subnodeOriginX))
                pplink2.set("locationY", str(round(subnodeOriginY + (NODESIZE - 1) / 2 + (counter + .25) * spacing)))
                pplink2.set("neighbor",  "AP" + actionPointIDs[counter])
                pplink2.set("intersection", "None")
 
                counter += 1
           
        else:
            #connection runs top to bottom - draw on x+ side
            counter = 0
            while(counter < actionPointCount):
                #draw each action point
                counterX = math.floor(subnodeOriginX + (lineWidth * 0.5) + 2)

                while(counterX < math.floor(subnodeOriginX + (lineWidth * 1.5) - 2)):
                    counterY = math.floor(counter * spacing + subnodeEndY + (NODESIZE - 1) / 2 + emptySpace)

                    while(counterY < (counter + 1) * spacing + subnodeEndY + (NODESIZE - 1) / 2 - emptySpace):
                        map[counterY][counterX] = actionPointColor
                        counterY +=1

                    counterX +=1

                #add action point as a node
                ppActionNode = ET.SubElement(ppData, "ppnode")
                ppActionNode.set("name", "AP" + actionPointIDs[counter])

                #add link from action point to end node
                pplink = ET.SubElement(ppActionNode, "pplink")
                pplink.set("cost", str((actionPointCount - counter - .5) * spacing + (NODESIZE / 2) + 1))
                pplink.set("locationX", str(subnodeOriginX))
                pplink.set("locationY", str(round(subnodeEndY + (NODESIZE - 1) / 2 + (counter + .75) * spacing)))
                pplink.set("neighbor", endSubNodeName)
                pplink.set("intersection", "None")


                #add links from action point to action point
                counter2 = counter + 1
                while(counter2 < actionPointCount):
                    pplink3 = ET.SubElement(ppActionNode, "pplink")
                    pplink3.set("cost", str((counter2 - counter) * spacing + 1))
                    pplink3.set("locationX", str(subnodeOriginX))
                    pplink3.set("locationY", str(round(subnodeEndY + (NODESIZE - 1) / 2 + (counter2 + .25) * spacing)))
                    pplink3.set("neighbor", "AP" + actionPointIDs[counter2])
                    pplink3.set("intersection", "None")

                    counter2 +=1

                #add link from origin to actionPoint
                pplink2 = ET.SubElement(ppOriginSubnode, "pplink")
                pplink2.set("cost", str((counter + .5) * spacing + (NODESIZE / 2) + 1))
                pplink2.set("locationX", str(subnodeOriginX))
                pplink2.set("locationY", str(round(subnodeEndY + (NODESIZE - 1) / 2 + (counter + .25) * spacing)))
                pplink2.set("neighbor",  "AP" + actionPointIDs[counter])
                pplink2.set("intersection", "None")


                counter += 1
    elif(subnodeOriginY == subnodeEndY):
        #horizontally algined nodes
        ymin = subnodeOriginY - lineWidth / 2
        ymax = subnodeOriginY + lineWidth / 2

        xmin = min(subnodeOriginX, subnodeEndX) - lineWidth / 2
        xmax = max(subnodeOriginX, subnodeEndX) + lineWidth / 2

        counterX = int(xmin)
        while(counterX <= xmax):
            counterY = int(ymin)

            while(counterY <= ymax):
                map[counterY][counterX] = connectionColor
                counterY += 1

            counterX += 1

        connectionLength = abs(subnodeEndX - subnodeOriginX)

        # add connection to path planning data
        #find the subnode to add the connection to 
        ppOriginSubnode = None
        for ppNode in ppData.findall("./ppnode"):
            if(ppNode.get("name") == originSubNodeName):
                ppOriginSubnode = ppNode

                #add pplink to subnode
                pplink = ET.SubElement(ppNode, "pplink")
                pplink.set("cost", str(connectionLength))
                pplink.set("locationX", str(round(subnodeOriginX + subnodeEndX) / 2))
                pplink.set("locationY", str(subnodeOriginY))   
                pplink.set("neighbor", endSubNodeName)
                pplink.set("intersection", "None")

        
        if not ppOriginSubnode:
            print("Cannot find path planning subnode - something is up!")
            return

        if(actionPointCount == 0):
            #don't try to draw action points if there aren't any
            return

        #draw action points evenly spaced
        #take off half a node size on each end add a quater of a node size to each end of action point as a buffer
        spacing, emptySpace = calculateActionPointSpacing(connectionLength, actionPointCount)

        #determine which side to draw action points
        if(subnodeOriginX < subnodeEndX):
            #connection runs left to right - draw on y+ side
            counter = 0
            while(counter < actionPointCount):
                #draw each action point
                counterY = math.floor(subnodeOriginY + (lineWidth * 0.5) + 2)

                while(counterY < math.floor(subnodeOriginY + (lineWidth * 1.5) - 2)):
                    counterX = math.floor(counter * spacing + subnodeOriginX + (NODESIZE - 1) / 2 + emptySpace)

                    while(counterX < (counter + 1) * spacing + subnodeOriginX + (NODESIZE - 1) / 2 - emptySpace):
                        map[counterY][counterX] = actionPointColor
                        counterX +=1

                    counterY +=1
                
                #add action point as a node
                ppActionNode = ET.SubElement(ppData, "ppnode")
                ppActionNode.set("name", "AP" + actionPointIDs[counter])

                #add link from action point to end node
                pplink = ET.SubElement(ppActionNode, "pplink")
                pplink.set("cost", str((actionPointCount - counter - .5) * spacing + (NODESIZE / 2) + 1))
                pplink.set("locationY", str(subnodeOriginY))
                pplink.set("locationX", str(round(subnodeOriginX + (NODESIZE - 1) / 2 + (counter + .75) * spacing)))
                pplink.set("neighbor", endSubNodeName)
                pplink.set("intersection", "None")

                #add links from action point to action point
                counter2 = counter + 1
                while(counter2 < actionPointCount):
                    pplink3 = ET.SubElement(ppActionNode, "pplink")
                    pplink3.set("cost", str((counter2 - counter) * spacing + 1))
                    pplink3.set("locationY", str(subnodeOriginY))
                    pplink3.set("locationX", str(round(subnodeOriginX + (NODESIZE - 1) / 2 + (counter2 + .25) * spacing)))
                    pplink3.set("neighbor", "AP" + actionPointIDs[counter2])
                    pplink3.set("intersection", "None")

                    counter2 +=1

                #add link from origin to actionPoint
                pplink2 = ET.SubElement(ppOriginSubnode, "pplink")
                pplink2.set("cost", str((counter + .5) * spacing + (NODESIZE / 2) + 1))
                pplink2.set("locationY", str(subnodeOriginY))
                pplink2.set("locationX", str(round(subnodeOriginX + (NODESIZE - 1) / 2 + (counter + .25) * spacing)))
                pplink2.set("neighbor",  "AP" + actionPointIDs[counter])
                pplink2.set("intersection", "None")


                counter += 1
           
        else:
            #connection runs right to left - draw on y- side
            counter = 0
            while(counter < actionPointCount):
                #draw each action point
                counterY = math.floor(subnodeOriginY - (lineWidth * 1.5) + 2)

                while(counterY < math.floor(subnodeOriginY - (lineWidth * 0.5) - 2)):
                    counterX = math.floor(counter * spacing + subnodeEndX + (NODESIZE - 1) / 2 + emptySpace)

                    while(counterX < (counter + 1) * spacing + subnodeEndX + (NODESIZE - 1) / 2 - emptySpace):
                        map[counterY][counterX] = actionPointColor
                        counterX +=1

                    counterY +=1

                #add action point as a node
                ppActionNode = ET.SubElement(ppData, "ppnode")
                ppActionNode.set("name", "AP" + actionPointIDs[counter])

                #add link from action point to end node
                pplink = ET.SubElement(ppActionNode, "pplink")
                pplink.set("cost", str((actionPointCount - counter - .5) * spacing + (NODESIZE / 2) + 1))
                pplink.set("locationY", str(subnodeOriginY))
                pplink.set("locationX", str(round(subnodeEndX + (NODESIZE - 1) / 2 + (counter + .75) * spacing)))
                pplink.set("neighbor", endSubNodeName)
                pplink.set("intersection", "None")

                #add links from action point to action point
                counter2 = counter + 1
                while(counter2 < actionPointCount):
                    pplink3 = ET.SubElement(ppActionNode, "pplink")
                    pplink3.set("cost", str((counter2 - counter) * spacing + 1))
                    pplink3.set("locationY", str(subnodeOriginY))
                    pplink3.set("locationX", str(round(subnodeEndX + (NODESIZE - 1) / 2 + (counter2 + .25) * spacing)))
                    pplink3.set("neighbor", "AP" + actionPointIDs[counter2])
                    pplink3.set("intersection", "None")

                    counter2 +=1

                #add link from origin to actionPoint
                pplink2 = ET.SubElement(ppOriginSubnode, "pplink")
                pplink2.set("cost", str((counter + .5) * spacing + (NODESIZE / 2) + 1))
                pplink2.set("locationY", str(subnodeOriginY))
                pplink2.set("locationX", str(round(subnodeEndX + (NODESIZE - 1) / 2 + (counter + .25) * spacing)))
                pplink2.set("neighbor",  "AP" + actionPointIDs[counter])    
                pplink2.set("intersection", "None")

                counter += 1
    else:
        print("Failed to draw connection. Connection not valid as sub nodes do not share one coordinate!")

def calculateActionPointSpacing(connectionLength, actionPointCount):

    #calculate the spacing between the nodes and actionpoints and the action points themselves
    if(connectionLength - (NODESIZE - 1) > 1.5 * (NODESIZE - 1) * actionPointCount): 
        #spacing between aps
        spacing = (connectionLength - (NODESIZE - 1)) / actionPointCount

    else:
        print("Failed to draw action points. Not enought room!")

    #spacing between nodes and ap
    emptySpace = round(max((spacing - NODESIZE) / 2 , 0))

    return spacing, emptySpace

def getNodeLocation(name, root):
    #get the location of a named node on the map
    locationX = -1
    locationY = -1
    for node in root.findall('./node'):
        if(node.get('name') == name):
            locationX = int(node.get('locationX'))
            locationY = int(node.get('locationY'))

    if(locationX < 0):
        print("No node found matching: " + str(name))

    return locationX, locationY

def getSubNodeLocation(name, root, lineWidth):
    #split name into position and node name
    nameArray = name.split("-")

    nodeLocationX, nodeLocationY = getNodeLocation(nameArray[0], root)

    match (nameArray[1]):
        case 'A':
            locationX = nodeLocationX + (NODESIZE - 1) / 8 * 3
            locationY = nodeLocationY + (NODESIZE - 1) / 2 + lineWidth
        case 'B':
            locationX = nodeLocationX + (NODESIZE - 1) / 2 + lineWidth
            locationY = nodeLocationY + (NODESIZE - 1) * 3 / 8
        case 'C':
            locationX = nodeLocationX + (NODESIZE - 1) / 2 + lineWidth
            locationY = nodeLocationY - (NODESIZE - 1) * 3/ 8
        case 'D':
            locationX = nodeLocationX + (NODESIZE - 1) / 8 * 3
            locationY = nodeLocationY - (NODESIZE - 1) / 2 - lineWidth
        case 'E':
            locationX = nodeLocationX - (NODESIZE - 1) / 8 * 3
            locationY = nodeLocationY - (NODESIZE - 1) / 2 - lineWidth
        case 'F':
            locationX = nodeLocationX - (NODESIZE - 1) / 2 - lineWidth
            locationY = nodeLocationY - (NODESIZE - 1) * 3 / 8
        case 'G':
            locationX = nodeLocationX - (NODESIZE - 1) / 2 - lineWidth
            locationY = nodeLocationY + (NODESIZE - 1) * 3 / 8
        case 'H':
            locationX = nodeLocationX - (NODESIZE - 1) / 8 * 3
            locationY = nodeLocationY + (NODESIZE - 1) / 2 + lineWidth
        case _:
            print("Invalid subnode code!")

    return locationX, locationY

def checkForActionPoints(root, name):
    #returns the number of action points along a connection

    nameArray = name.split("-")
    
    connection = None
    for node in root.findall("./node"):
        if(node.get("name") == nameArray[0]):
            for subnode in node.findall("./subnode"):
                if(subnode.get('name') == name):
                    for connection in subnode.findall('./connection'):
                        ids = []
                        for actionPoint in connection.findall("./action_point"):
                            ids.append(actionPoint.get("id"))
                        
                        if(len(ids) != 0):
                            return ids

    #return zero if this fails for any case         
    return []

def calculateConnectionDrawingPoint(startSubnodeName, endSubnodeName, root):
    #get starting node location
    startX, startY = getSubNodeLocation(startSubnodeName, root, getConnectionLineWidth())

    #get ending node location
    endX, endY = getSubNodeLocation(endSubnodeName, root, getConnectionLineWidth())

    direction = ""
    if(startX == endX):
        #if a vertical connection
        if(startY > endY):
            direction = "YMinus"
        else:
            direction = "YPlus"
    else:
        #if a horizontal connection
        if(startX > endX):
            direction = "XMinus"
        else:
            direction = "XPlus"

    #get the average of the two - in theory the coordinates in one direction should be the same
    return (endX + startX) / 2, (startY + endY) / 2, direction 

def calculateActionPointDrawingPoint(startSubnodeName, endSubnodeName, root, actionPointNumber, actionPointCount):
    #get starting node location
    startX, startY = getSubNodeLocation(startSubnodeName, root, getConnectionLineWidth())

    #get ending node location
    endX, endY = getSubNodeLocation(endSubnodeName, root, getConnectionLineWidth())

    #defaults x = 0, y = 0
    x = 0
    y = 0
    

    if(startX == endX):
        #if a vertical connection
        x = startX

        if(actionPointCount == 1):
            #in the middle of the connection
            y = (startY + endY) / 2

        else:
            #space with the action points
            if(startY > endY):
                #using algorithm used to draw action points
                spacing, emptySpace = calculateActionPointSpacing(startY - endY, actionPointCount)
                y = startY - spacing * (actionPointNumber + .5) + (NODESIZE - 1) + emptySpace
            else:
                spacing, emptySpace = calculateActionPointSpacing(endY - startY, actionPointCount)
                y = startY + spacing * (actionPointNumber + .5) - (NODESIZE - 1) * 2 + emptySpace
    else:
        #if a horizontal connection
        y = startY

        if(actionPointCount == 1):
            #in the middle of the connection
            x = (startX + endX) / 2

        else:
            #space with the action points
            if(startX > endX):
                #using algorithm used to draw action points
                spacing, emptySpace = calculateActionPointSpacing(startX - endX, actionPointCount)
                x = startX - spacing * (actionPointNumber + .5) + (NODESIZE - 1) / 2
            else:
                spacing, emptySpace = calculateActionPointSpacing(endX - startX, actionPointCount)
                x = startX + spacing * (actionPointNumber + .5) - (NODESIZE - 1) / 2

    #get the average of the two - in theory the coordinates in one direction should be the same
    return x, y, "station"

def getSubnodeDrawingDirection(subnodeName):
    #return the direction a bot should be drawn for any given subnode

    nameArray = subnodeName.split("-")

    if(len(nameArray) != 2):
        print("Cannot determine bot direction! Subnode name invalid: " + subnodeName)
        return
    
    #determine direction based on subnode part of name
    direction = ""
    match nameArray[1]:
        case "A":
            return "YPlus"       
        case "B":
            return "XMinus"       
        case "C":
            return "XPlus"       
        case "D":
            return "YPlus"       
        case "E":
            return "YMinus"       
        case "F":
            return "XPlus"       
        case "G":
            return "XMinus"
        case "H":
            return "YMinus"
        case _:
            print("Cannot determine bot direction! Subnode name invalid: " + subnodeName)

        
    return direction


def drawBots(imageMap, botLocations):
    #draws the bot locations onto an already premade map

    #ensure the positions dictionary has been loaded
    if(len(botDrawingLocations.keys()) == 0):
        print("Cannot draw bots! Bot drawing locations must be loaded first!")

    #iterate through the bots
    for locationKey in botLocations:
        if botLocations[locationKey] in botDrawingLocations.keys():
        
            pixelLocation = botDrawingLocations[botLocations[locationKey]]

            #load in bot image
            #TODO: get name resolution working
            botImageArray = np.array(np.load(os.path.join(botImagesPath, "bitmap", locationKey + pixelLocation[2] + ".npy")))

            #size of bot image (y,x,p)
            botSize = botImageArray.shape
            
            #copy bot image onto map image
            yMin = int(pixelLocation[1] - math.ceil(botSize[0] / 2))
            xMin = int(pixelLocation[0] - math.ceil(botSize[1] / 2))

            yCounter = 0
            while(yCounter < botSize[0]):

                xCounter = 0
                while(xCounter < botSize[1]):
                    #ensure it does not draw transluent pixels
                    if(not np.array_equal(botImageArray[yCounter][xCounter], np.array([0,0,0,0]))):
                        imageMap[yCounter + yMin][xCounter + xMin] = botImageArray[yCounter][xCounter]
                    xCounter += 1

                yCounter += 1
        elif not (botLocations[locationKey] == "Unlocalized"):
            #if there is not match for the location in the locations and the bot is not unlocalized
            print("Cannot draw bot! Invalid location: " + str(locationKey) + ", ")
            return
        
        
    return flipImageY(imageMap)

if __name__ == "__main__":
    #run this file to reload map.xml

    print("Loading In Map!")

    file = openXMLFile()
    g_mapRoot = parseFile(file)

    if (g_mapRoot):
        displayBaseMap(g_mapRoot)
