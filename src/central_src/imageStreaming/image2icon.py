from PIL import Image
import numpy as np
import os
import math

def image2icon(iconName, size: int):
    #iconName = string representing the icon
    #size = the size of the icon - always a square

    filePath = os.path.dirname(os.path.realpath(__file__))
    iconPath = os.path.join(filePath, "rawImages", "test_icon_" + iconName + ".png")
    iconImage = Image.open(iconPath)

    iconRawBitmap = np.asarray(iconImage)

    squareBitmap = []
    #adjust bitmap to aspect ration by adding pixels to top or sides
    if(len(iconRawBitmap) > len(iconRawBitmap[0])):
        #add pixels to sides
        columnsNeeded = len(iconRawBitmap) - len(iconRawBitmap[0])
        leftAdd = math.ceil(columnsNeeded / 2)

        squareBitmap = np.zeros(shape=(len(iconRawBitmap), len(iconRawBitmap), 4))

        counter = 0
        for row in iconRawBitmap:

            innerCounter = 0
            while(counter < len(iconRawBitmap[0])):
                squareBitmap[counter][innerCounter + leftAdd] = row[innerCounter]

                innerCounter += 1

            counter += 1

    elif (len(iconRawBitmap) < len(iconRawBitmap[0])):
        #add pixels to top and bottom
        rowsNeededTop = math.ceil((len(iconRawBitmap[0]) - len(iconRawBitmap)) / 2)
        
        squareBitmap = np.zeros(shape=(len(iconRawBitmap[0]), len(iconRawBitmap[0]), 4))

        counter = 0
        for row in iconRawBitmap:
            squareBitmap[rowsNeededTop + counter] = row
            counter +=1

    else:
        #perfectly square to start with
        squareBitmap = iconRawBitmap

    #scale image
    invScale = len(squareBitmap) / size
    
    #get a list of key pixel coordinate to scale with
    keyCoordinates = []
    position = invScale * .5
    while(position < len(squareBitmap)):

        keyCoordinates.append(round(position))
        position += invScale


    #create a scaled bitmap
    scaledBitmap = np.zeros((size, size, 4))
    
    counter = 0
    for y in keyCoordinates:

        innerCounter = 0
        for x in keyCoordinates:
            scaledBitmap[counter][innerCounter] = squareBitmap[y][x]
            innerCounter += 1

        counter += 1

    icon = Image.fromarray(np.uint8(scaledBitmap))

    #save image
    imageSavePath = os.path.join(filePath, "icons", "icon_" + iconName + "_" + str(size) + ".png")
    icon.save(imageSavePath)

    #save icon as packets
    counter = 0
    innerCounter = 0
    packetCounter = 0
    rowsPerPacket = math.floor((1000 - 100) / (len(scaledBitmap[0]) * 2))

    packetBytes = []
    while(True):
        #stop at end of image
        if(counter >= len(scaledBitmap)):
            #write last packet to file
            file = open(os.path.join(filePath, "packets", iconName + "_" + str(size) + "_" + str(packetCounter) + ".bin"), "wb")
            
            file.write(bytearray(packetBytes))
            file.close

            packetBytes = []
            packetCounter += 1
            innerCounter = 0

            break

        #control packet grouping
        if(innerCounter >= rowsPerPacket):
            #write packet to file
            file = open(os.path.join(filePath, "packets", iconName + "_" + str(size) + "_" + str(packetCounter) + ".bin"), "wb")

            file.write(bytearray(packetBytes))
            file.close

            packetBytes = []
            packetCounter += 1
            innerCounter = 0

        #add rows to packet
        for pixel in scaledBitmap[counter]:
            b1, b2 = rgb888to565(pixel)
            
            packetBytes.append(b1)
            packetBytes.append(b2)

        counter += 1
        innerCounter += 1
    

def rgb888to565(pixel):
    #cannot find better code online - wierd
    b1 = 0
    b2 = 0

    # may have to do some color balancing
    r5 = pixel[0] / 8
    g6 = pixel[1] / 4
    b5 = pixel[2] / 8

    #byte 1 string
    if(r5 >= 16):
        r5 -= 16
        b1 += 128
 
    
    if(r5 >= 8):
        r5 -= 8
        b1 += 64
  
    
    if(r5 >= 4):
        r5 -= 4
        b1 += 32
  
    
    if(r5 >= 2):
        r5 -= 2
        b1 += 16
  
        
    if(r5 >= 1):
        r5 -= 1
        b1 += 8
  
        
    if(g6 >= 32):
        r5 -= 32
        b1 += 4


    if(g6 >= 16):
        r5 -= 16
        b1 += 2


    if(g6 >= 8):
        r5 -= 8
        b1 += 1


    #byte 2 string
    if(g6 >= 4):
        g6 -= 4
        b2 += 128


    if(g6 >= 2):
        g6 -= 2
        b2 += 64


    if(g6 >= 1):
        g6 -= 1
        b2 += 32

    
    b2 += int(b5)

    return b1, b2


if __name__ == "__main__":
    #test parse the jeep icon
    image2icon("power", 18)
