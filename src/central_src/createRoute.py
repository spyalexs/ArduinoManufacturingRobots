import xml.etree.ElementTree as ET
import os

def loadPPData():
    #open xml file and get root
    xmlFilePath = os.path.dirname(os.path.realpath(__file__))
    file = open(xmlFilePath, 'r')
    root = ET.parse(file).getroot()

    return root