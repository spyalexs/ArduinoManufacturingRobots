import numpy as np
from PIL import Image
import os
from math import floor

def generateBot(name, color):
    #name should be the name of the bot
    #color shuuld be rgba

    #draw two rectangles that represent a bot
    #12 by 12
    array = drawBot(12, 12, color)
    
    #get target directory
    path = os.path.join(os.path.dirname(os.path.realpath(__file__)), "botimages")

    #save image in viewable format
    image = Image.fromarray(np.uint8(array))   
    image.save(os.path.join(path, name + ".png"))

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

    shaftHieght = arrowHeight - pointHeight
    shaftWidth = round(pointWidth / 2)
    if(shaftWidth % 2 == 1):
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
        while(xCounter < round(xSize - (xsize - shaftWidth) / 2)):
            array[yCounter][xCounter] = [255, 255, 255, 255]
            xCounter +=1

        yCounter += 1

    return array

if __name__ == "__main__":
    generateBot("bot3", [0, 255, 0, 255])

    




