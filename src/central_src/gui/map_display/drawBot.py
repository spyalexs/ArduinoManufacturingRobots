import numpy as np
from PIL import Image
import os
from math import floor
from random import random

def generateBot(name, color):
    #name should be the name of the bot
    #color shuuld be rgba

    #draw two rectangles that represent a bot
    #should be a square
    array = np.uint8(drawBot(15, 15, color))

    #create four arrays for four different orientations
    yPlus = array
    yMinus = np.flip(array, 0)
    xMinus = np.flip(np.rot90(array), 1)
    xPlus = np.rot90(array)
    
    #get target directory
    path = os.path.join(os.path.dirname(os.path.realpath(__file__)), "botimages")

    #save images as bitmaps
    np.save(os.path.join(path, "bitmap", name + "YPlus.npy"), yPlus)
    np.save(os.path.join(path, "bitmap", name + "YMinus.npy"), yPlus)
    np.save(os.path.join(path, "bitmap", name + "XPlus.npy"), yPlus)
    np.save(os.path.join(path, "bitmap", name + "XMinus.npy"), yPlus)

    #save images in viewable format
    image = Image.fromarray(yPlus)   
    image.save(os.path.join(path, "image", name + "YPlus.png"))    
    image = Image.fromarray(yMinus)   
    image.save(os.path.join(path, "image", name + "YMinus.png"))    
    image = Image.fromarray(xPlus)   
    image.save(os.path.join(path, "image", name + "XPlus.png"))    
    image = Image.fromarray(xMinus)   
    image.save(os.path.join(path, "image", name + "XMinus.png"))

def drawBot(xSize, ySize, color):
    if(xSize < 5 or ySize < 5):
        print("Cannot draw bot, it must be at least 5 by in each dimension. Current size is X: " + xSize + " Y: " + ySize)
        return None
    
    xsize = round(xSize)
    ysize = round(ySize)
    
    array = np.zeros(shape=(xsize, ysize,4))

    #sent most of bot to be the main color
    yCounter = 0
    while yCounter < len(array):
        xCounter = 0
        while xCounter < len(array[yCounter]):
            array[yCounter][xCounter] = color
            
            xCounter += 1

        yCounter += 1

    #round the corners
    array[0][0] = [0,0,0,0]
    array[1][0] = [0,0,0,0]
    array[0][1] = [0,0,0,0]
    array[len(array) - 1][0] = [0,0,0,0]
    array[len(array) - 2][0] = [0,0,0,0]
    array[len(array) - 1][1] = [0,0,0,0]
    array[0][len(array[0]) - 1] = [0,0,0,0]
    array[1][len(array[0]) - 1] = [0,0,0,0]
    array[0][len(array[0]) - 2] = [0,0,0,0]
    array[len(array) - 1][len(array[0]) - 1] = [0,0,0,0]
    array[len(array) - 2][len(array[0]) - 1] = [0,0,0,0]
    array[len(array) - 1][len(array[0]) - 2] = [0,0,0,0]

    if(xSize < 12 or ySize < 12):
        #dont attempt to render directionality arrow on less than 12 by 12
        return array
    
    #get dimension for point of arrow
    pointWidth = round(xSize * 0.6666)
    #ensure arrow is always even
    if((xsize - pointWidth) % 2 == 1):
        pointWidth -= 1

    pointHeight = floor(pointWidth / 2)
    if(pointHeight % 2 == 1):
        #if a one pixel point add another to height
        pointHeight += 1

    #get shaft of array height
    arrowHeight = round(ySize * 0.6666)
    if((ysize - arrowHeight) % 2 == 1):
        arrowHeight -= 1

    shaftWidth = round(pointWidth / 2)
    if((xsize - shaftWidth) % 2 == 1):
        shaftWidth -= 1

    #draw point
    yCounter = round(len(array) - pointHeight - ((ysize - arrowHeight) / 2))
    step = 0

    while (step <= pointWidth / 2):

        xCounter = round(step + (xsize - pointWidth) / 2)
        while(xCounter < round(xsize - step - (xsize - pointWidth) / 2)):
            array[yCounter][xCounter] = [255, 255, 255, 255]
            xCounter +=1

        yCounter += 1
        step += 1

    yCounter = round((ysize - arrowHeight) / 2)

    while(yCounter < round(len(array) - pointHeight - ((ysize - arrowHeight) / 2))):
 
        xCounter = round((xsize - shaftWidth) / 2)
        while(xCounter < round(xsize - (xsize - shaftWidth) / 2)):
            array[yCounter][xCounter] = [255, 255, 255, 255]
            xCounter +=1

        yCounter += 1

    return array

def getRandomColor():
    #return a random rgba color
    return [round(random() * 255), round(random() * 255), round(random() * 255), 255]

if __name__ == "__main__":
    #run this file to generate bot art

    generateBot("5010", [0, 255, 0, 255])
    generateBot("5011", [255, 154, 138, 255])
    generateBot("5012", [255, 165, 0, 255])
    generateBot("5013", [255, 0, 255, 255])
    generateBot("5014", [192, 192, 192, 255])
    generateBot("5015", [210, 180, 140, 255])

    #after these set colors do some random ones - really only should be six in use
    generateBot("5016", getRandomColor())    
    generateBot("5017", getRandomColor())    
    generateBot("5018", getRandomColor())    
    generateBot("5019", getRandomColor())    
    generateBot("5020", getRandomColor())    
    generateBot("5021", getRandomColor())    
    generateBot("5022", getRandomColor())    
    generateBot("5023", getRandomColor())    
    generateBot("5024", getRandomColor())    
    generateBot("5025", getRandomColor())    
    generateBot("5026", getRandomColor())    
    generateBot("5027", getRandomColor())    
    generateBot("5028", getRandomColor())    
    generateBot("5029", getRandomColor())    




