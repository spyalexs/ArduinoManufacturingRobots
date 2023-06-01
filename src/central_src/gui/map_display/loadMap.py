import xml.etree.ElementTree as ET
import os

from displayMap import displayMap

MAPFILENAME = "map.xml"
g_mapRoot = None

def parseFile(file):
    mapTree = ET.parse(file)
    return mapTree.getroot()

def openXMLFile():
    #opens the xml file containing the map
    filePath = os.path.join(os.path.dirname(os.path.realpath(__file__)), MAPFILENAME)

    return open(filePath, 'r')

if __name__ == "__main__":
    print("Loading In Map!")

    file = openXMLFile()
    g_mapRoot =parseFile(file)

    displayMap(g_mapRoot)



