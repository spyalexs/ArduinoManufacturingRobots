import PySimpleGUI as sg
from PIL import Image, ImageTk
import xml.etree.ElementTree as ET
import numpy as np
import sys
import os
import math

#add theme to path
sys.path.insert(1, os.path.join(os.path.dirname(os.path.realpath(__file__)), "..", ".."))
from gui.monitorGUI import getTheme

NODESIZE = 33 #mutliple of 8 + 1 and must be at leatst 25

def displayMap(root):
    #get the theme from the other display to ensure they match
    sg.theme(getTheme())

    #set the root loaded from xml
    layout = [[sg.Image(size=(600,600), key="mapImage")]]

    window = sg.Window(title="Map", layout=layout, finalize=True)
    window["mapImage"].update(data=drawMap(root))
    window.read()

def loadImage():
    #load a test imahe
    img = Image.open(r".\gui\map_display\test.jpg")
    return ImageTk.PhotoImage(img)

def prepPathPlanningXML(root):
    #prep a path planning xml by finding and listing all sub nodes and intra node connections

    #xml data container
    ppData = ET.Element("map")

    intraConnectionCost = 30 # the cost of making an intranode connection

    #iterate over all subnodes

    for node in root.findall("./node"):
        for subnode in node.findall("./subnode"):
            ppnode = ET.SubElement(ppData, "ppnode")

            #save pp node data
            ppnodeName = subnode.get("name")
            ppNodeNameArray = ppnodeName.split("-")
            ppnode.set("name", ppnodeName)

            #create and save all of the valid intranode connections from the subnode
            if(ppNodeNameArray[1] == 'A' or ppNodeNameArray[1] == 'C' or ppNodeNameArray[1] == 'E' or ppNodeNameArray[1] == 'G'):
                pplink1 = ET.SubElement(ppnode, "pplink")
                pplink1.set("cost", str(intraConnectionCost))
                pplink1.set("intersection", ppNodeNameArray[0])
                pplink1.set("locationX", node.get("locationX"))
                pplink1.set("locationY", node.get("locationY"))   
                pplink1.set("neighbor", ppNodeNameArray[0] + "-B")             
                pplink2 = ET.SubElement(ppnode, "pplink")
                pplink2.set("cost", str(intraConnectionCost))
                pplink2.set("intersection", ppNodeNameArray[0])
                pplink2.set("locationX", node.get("locationX"))
                pplink2.set("locationY", node.get("locationY"))
                pplink2.set("neighbor", ppNodeNameArray[0] + "-D")             
                pplink3 = ET.SubElement(ppnode, "pplink")
                pplink3.set("cost", str(intraConnectionCost))
                pplink3.set("intersection", ppNodeNameArray[0])
                pplink3.set("locationX", node.get("locationX"))
                pplink3.set("locationY", node.get("locationY"))
                pplink3.set("neighbor", ppNodeNameArray[0] + "-F")             
                pplink4 = ET.SubElement(ppnode, "pplink")
                pplink4.set("cost", str(intraConnectionCost))
                pplink4.set("intersection", ppNodeNameArray[0])
                pplink4.set("locationX", node.get("locationX"))
                pplink4.set("locationY", node.get("locationY"))
                pplink4.set("neighbor", ppNodeNameArray[0] + "-H")           
            else:
                pplink1 = ET.SubElement(ppnode, "pplink")
                pplink1.set("cost", str(intraConnectionCost))
                pplink1.set("intersection", ppNodeNameArray[0])
                pplink1.set("locationX", node.get("locationX"))
                pplink1.set("locationY", node.get("locationY"))   
                pplink1.set("neighbor", ppNodeNameArray[0] + "-A")             
                pplink2 = ET.SubElement(ppnode, "pplink")
                pplink2.set("cost", str(intraConnectionCost))
                pplink2.set("intersection", ppNodeNameArray[0])
                pplink2.set("locationX", node.get("locationX"))
                pplink2.set("locationY", node.get("locationY"))
                pplink2.set("neighbor", ppNodeNameArray[0] + "-C")             
                pplink3 = ET.SubElement(ppnode, "pplink")
                pplink3.set("cost", str(intraConnectionCost))
                pplink3.set("intersection", ppNodeNameArray[0])
                pplink3.set("locationX", node.get("locationX"))
                pplink3.set("locationY", node.get("locationY"))
                pplink3.set("neighbor", ppNodeNameArray[0] + "-E")             
                pplink4 = ET.SubElement(ppnode, "pplink")
                pplink4.set("cost", str(intraConnectionCost))
                pplink4.set("intersection", ppNodeNameArray[0])
                pplink4.set("locationX", node.get("locationX"))
                pplink4.set("locationY", node.get("locationY"))
                pplink4.set("neighbor", ppNodeNameArray[0] + "-F")  

    return ppData

def drawMap(root):
    map = np.zeros(shape=(300,300,4))
    # map[row][column]

    #get the initial path planning data
    ppData = prepPathPlanningXML(root)

    for node in root.findall("./node"):
        #draw in nodes
        name = node.get('name')
        nodeX, nodeY = getNodeLocation(name, root)
        drawNode(nodeX, nodeY, map)

        #draw in connections 
        for subnode in node.findall("./subnode"):
            subnodeName = subnode.get('name')
            for connection in subnode.findall('./connection'):
                endName = connection.get("destination")

                drawConnection(map, root, subnodeName, endName, ppData)

    #write xml generated to file
    ppTree = ET.ElementTree(ppData)
    ET.indent(ppTree, " ")
    ppTree.write("test.xml")

    mapImage = Image.fromarray(np.uint8(map))
    return ImageTk.PhotoImage(mapImage)

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

def drawConnection(map, root, originSubNodeName, endSubNodeName, ppData):
    #connection line width 
    lineWidth = (NODESIZE - 1) / 4

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
                pplink.set("neighbor", endSubNodeName)
        
        if not ppOriginSubnode:
            print("Cannot find path planning subnode - something is up!")
            return

        if(actionPointCount == 0):
            #don't try to draw action points if there aren't any
            return

        #draw action points evenly spaced
        #take off half a node size on each end add a quater of a node size to each end of action point as a buffer
        spacing = 0
        if(connectionLength - (NODESIZE - 1) > 1.5 * (NODESIZE - 1) * actionPointCount):
            spacing = (connectionLength - (NODESIZE - 1)) / actionPointCount
        else:
            print("Failed to draw action points. Not enought room!")

        emptySpace = round(max((spacing - NODESIZE) / 2 , 0))

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
                ppActionNode.set("name", "actionpoint" + actionPointIDs[counter])

                #add link from action point to end node
                pplink = ET.SubElement(ppActionNode, "pplink")
                pplink.set("cost", str((actionPointCount - counter - .5) * spacing + 1))
                pplink.set("locationX", str(subnodeOriginX))
                pplink.set("locationY", str(round(subnodeOriginY + (NODESIZE - 1) / 2 + (counter + .75) * spacing)))
                pplink.set("neighbor", endSubNodeName)

                #add links from action point to action point
                counter2 = counter + 1
                while(counter2 < actionPointCount):
                    pplink = ET.SubElement(ppActionNode, "pplink")
                    pplink.set("cost", str((counter2 - counter - .5) * spacing + 1))
                    pplink.set("locationX", str(subnodeOriginX))
                    pplink.set("locationY", str(round(subnodeOriginY + (NODESIZE - 1) / 2 + (counter2 + .25) * spacing)))
                    pplink.set("neighbor", "actionpoint" + actionPointIDs[counter2])
                    counter2 +=1

                #add link from origin to actionPoint
                pplink2 = ET.SubElement(ppOriginSubnode, "pplink")
                pplink2.set("cost", str((counter + .5) * spacing + 1))
                pplink2.set("locationX", str(subnodeOriginX))
                pplink2.set("locationY", str(round(subnodeOriginY + (NODESIZE - 1) / 2 + (counter + .25) * spacing)))
                pplink2.set("neighbor",  "actionpoint" + actionPointIDs[counter])
                
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
                ppActionNode.set("name", "actionpoint" + actionPointIDs[counter])

                #add link from action point to end node
                pplink = ET.SubElement(ppActionNode, "pplink")
                pplink.set("cost", str((actionPointCount - counter - .5) * spacing + 1))
                pplink.set("locationX", str(subnodeOriginX))
                pplink.set("locationY", str(round(subnodeEndY + (NODESIZE - 1) / 2 + (counter + .75) * spacing)))
                pplink.set("neighbor", endSubNodeName)

                #add links from action point to action point
                counter2 = counter + 1
                while(counter2 < actionPointCount):
                    pplink = ET.SubElement(ppActionNode, "pplink")
                    pplink.set("cost", str((counter2 - counter - .5) * spacing + 1))
                    pplink.set("locationX", str(subnodeOriginX))
                    pplink.set("locationY", str(round(subnodeEndY + (NODESIZE - 1) / 2 + (counter2 + .25) * spacing)))
                    pplink.set("neighbor", "actionpoint" + actionPointIDs[counter2])
                    counter2 +=1

                #add link from origin to actionPoint
                pplink2 = ET.SubElement(ppOriginSubnode, "pplink")
                pplink2.set("cost", str((counter + .5) * spacing + 1))
                pplink2.set("locationX", str(subnodeOriginX))
                pplink2.set("locationY", str(round(subnodeEndY + (NODESIZE - 1) / 2 + (counter + .25) * spacing)))
                pplink2.set("neighbor",  "actionpoint" + actionPointIDs[counter])

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
        
        if not ppOriginSubnode:
            print("Cannot find path planning subnode - something is up!")
            return

        if(actionPointCount == 0):
            #don't try to draw action points if there aren't any
            return

        #draw action points evenly spaced
        #take off half a node size on each end add a quater of a node size to each end of action point as a buffer
        spacing = 0
        if(connectionLength - (NODESIZE - 1) > 1.5 * (NODESIZE - 1) * actionPointCount):
            spacing = (connectionLength - (NODESIZE - 1)) / actionPointCount
        else:
            print("Failed to draw action points. Not enought room!")

        emptySpace = round(max((spacing - NODESIZE) / 2 , 0))

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
                ppActionNode.set("name", "actionpoint" + actionPointIDs[counter])

                #add link from action point to end node
                pplink = ET.SubElement(ppActionNode, "pplink")
                pplink.set("cost", str((actionPointCount - counter - .5) * spacing + 1))
                pplink.set("locationY", str(subnodeOriginY))
                pplink.set("locationX", str(round(subnodeOriginX + (NODESIZE - 1) / 2 + (counter + .75) * spacing)))
                pplink.set("neighbor", endSubNodeName)

                #add links from action point to action point
                counter2 = counter + 1
                while(counter2 < actionPointCount):
                    pplink = ET.SubElement(ppActionNode, "pplink")
                    pplink.set("cost", str((counter2 - counter - .5) * spacing + 1))
                    pplink.set("locationY", str(subnodeOriginY))
                    pplink.set("locationX", str(round(subnodeOriginX + (NODESIZE - 1) / 2 + (counter2 + .25) * spacing)))
                    pplink.set("neighbor", "actionpoint" + actionPointIDs[counter2])
                    counter2 +=1

                #add link from origin to actionPoint
                pplink2 = ET.SubElement(ppOriginSubnode, "pplink")
                pplink2.set("cost", str((counter + .5) * spacing + 1))
                pplink2.set("locationY", str(subnodeOriginY))
                pplink2.set("locationX", str(round(subnodeOriginX + (NODESIZE - 1) / 2 + (counter + .25) * spacing)))
                pplink2.set("neighbor",  "actionpoint" + actionPointIDs[counter])

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
                ppActionNode.set("name", "actionpoint" + actionPointIDs[counter])

                #add link from action point to end node
                pplink = ET.SubElement(ppActionNode, "pplink")
                pplink.set("cost", str((actionPointCount - counter - .5) * spacing + 1))
                pplink.set("locationY", str(subnodeOriginY))
                pplink.set("locationX", str(round(subnodeEndX + (NODESIZE - 1) / 2 + (counter + .75) * spacing)))
                pplink.set("neighbor", endSubNodeName)

                #add links from action point to action point
                counter2 = counter + 1
                while(counter2 < actionPointCount):
                    pplink = ET.SubElement(ppActionNode, "pplink")
                    pplink.set("cost", str((counter2 - counter - .5) * spacing + 1))
                    pplink.set("locationY", str(subnodeOriginY))
                    pplink.set("locationX", str(round(subnodeEndX + (NODESIZE - 1) / 2 + (counter2 + .25) * spacing)))
                    pplink.set("neighbor", "actionpoint" + actionPointIDs[counter2])
                    counter2 +=1

                #add link from origin to actionPoint
                pplink2 = ET.SubElement(ppOriginSubnode, "pplink")
                pplink2.set("cost", str((counter + .5) * spacing + 1))
                pplink2.set("locationY", str(subnodeOriginY))
                pplink2.set("locationX", str(round(subnodeEndX + (NODESIZE - 1) / 2 + (counter + .25) * spacing)))
                pplink2.set("neighbor",  "actionpoint" + actionPointIDs[counter])

                counter += 1
    else:
        print("Failed to draw connection. Connection not valid as sub nodes do not share one coordinate!")

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
