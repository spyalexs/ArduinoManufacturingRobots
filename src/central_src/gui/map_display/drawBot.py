import numpy as np
from PIL import Image
import os

def generateBot(name, color):
    #name should be the name of the bot
    #color shuuld be rgba

    #draw two rectangles that represent a bot
    #12 by 12

    #blank image
    array = np.zeros(shape=(12,12,4))

    #upper box
    ycounter = 0
    while ycounter < 6:
        xcounter = 3

        while xcounter < 9:
            array[ycounter][xcounter] = color
            xcounter += 1

        ycounter += 1

    #lower box
    ycounter = 6
    while ycounter < 12:
        xcounter = 0

        while xcounter < 12:
            array[ycounter][xcounter] = color
            xcounter += 1

        ycounter += 1

    #get target directory
    path = os.path.join(os.path.dirname(os.path.realpath(__file__)), "botimages")

    #save image in viewable format
    image = Image.fromarray(np.uint8(array))   
    image.save(os.path.join(path, name + ".png"))


if __name__ == "__main__":
    generateBot("bot3", [0, 255, 0, 255])

    




