import PySimpleGUI as sg
from PIL import Image, ImageTk
import numpy as np
import sys
import os

#add theme to path
sys.path.insert(1, os.path.join(os.path.dirname(os.path.realpath(__file__)), "..", ".."))
from gui.monitorGUI import getTheme

NODESIZE = 41 #mutliple of 8 + 1

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

def drawMap(root):
    map = np.zeros(shape=(300,300,4))
    # map[row][column]

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

                drawConnection(map, root, subnodeName, endName)


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

def drawConnection(map, root, subNode1Name, subNode2Name):
    #connection line width 
    lineWidth = (NODESIZE - 1) / 4

    #color to draw connections
    connectionColor = [0,0,0, 255]

    #check to ensure the node path is not between two subnodes of the same node
    if(subNode1Name.split("-")[0] == subNode2Name.split("-")[0]):
        print("no intra subnode connections")
        return

    #subnode 1 location
    subnode1X, subnode1Y = getSubNodeLocation(subNode1Name, root, NODESIZE, lineWidth)

    #subnode 2 location
    subnode2X, subnode2Y = getSubNodeLocation(subNode2Name, root, NODESIZE, lineWidth) 

    if(subnode1X == subnode2X):
        #vertically alginded nodes
        xmin = subnode1X - lineWidth / 2
        xmax = subnode1X + lineWidth / 2

        ymin = min(subnode1Y, subnode2Y) - lineWidth / 2
        ymax = max(subnode1Y, subnode2Y) + lineWidth / 2

        counterX = int(xmin)
        while(counterX <= xmax):
            counterY = int(ymin)

            while(counterY <= ymax):
                map[counterY][counterX] = connectionColor
                counterY += 1

            counterX += 1

    elif(subnode1Y == subnode2Y):
        #horizontally algined nodes
        ymin = subnode1Y - lineWidth / 2
        ymax = subnode1Y + lineWidth / 2

        xmin = min(subnode1X, subnode2X) - lineWidth / 2
        xmax = max(subnode1X, subnode2X) + lineWidth / 2

        counterX = int(xmin)
        while(counterX <= xmax):
            counterY = int(ymin)

            while(counterY <= ymax):
                map[counterY][counterX] = connectionColor
                counterY += 1

            counterX += 1
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

def getSubNodeLocation(name, root, nodeSize, lineWidth):
    #split name into position and node name
    nameArray = name.split("-")

    nodeLocationX, nodeLocationY = getNodeLocation(nameArray[0], root)

    match (nameArray[1]):
        case 'A':
            locationX = nodeLocationX + nodeSize / 8 * 3
            locationY = nodeLocationY + nodeSize / 2 + lineWidth
        case 'B':
            locationX = nodeLocationX + nodeSize / 2 + lineWidth
            locationY = nodeLocationY + nodeSize * 3 / 8
        case 'C':
            locationX = nodeLocationX + nodeSize / 2 + lineWidth
            locationY = nodeLocationY - nodeSize * 3/ 8
        case 'D':
            locationX = nodeLocationX + nodeSize / 8 * 3
            locationY = nodeLocationY - nodeSize / 2 - lineWidth
        case 'E':
            locationX = nodeLocationX - nodeSize / 8 * 3
            locationY = nodeLocationY - nodeSize / 2 - lineWidth
        case 'F':
            locationX = nodeLocationX - nodeSize / 2 - lineWidth
            locationY = nodeLocationY - nodeSize * 3 / 8
        case 'G':
            locationX = nodeLocationX - nodeSize / 2 - lineWidth
            locationY = nodeLocationY + nodeSize * 3 / 8
        case 'H':
            locationX = nodeLocationX - nodeSize / 8 * 3
            locationY = nodeLocationY + nodeSize / 2 + lineWidth
        case _:
            print("Invalid subnode code!")

    return locationX, locationY

