import xml.etree.ElementTree as ET
import os
from random import random

#class to hold open node data
class PPNode:
    m_cost = -1
    m_parentalStructure = []
    m_name = ""
    m_node = None
    m_id = 0

    def __init__ (self, node, parentalStructure, cost, name):
        self.m_name = name
        self.m_cost = cost
        self.m_parentalStructure = parentalStructure.copy()
        self.m_parentalStructure.append(name)
        self.m_node = node

        self.m_id = round(random() * 1000000)

    def reroute(self, parentalStructure, cost):
        self.m_cost = cost
        self.m_parentalStructure = parentalStructure.copy()
        self.m_parentalStructure.append(self.m_name)

def loadPPData():
    #open xml file and get root
    xmlFilePath = os.path.join(os.path.dirname(os.path.realpath(__file__)), "ppData.xml")
    file = open(xmlFilePath, 'r')
    root = ET.parse(file).getroot()

    return root

def getRoute(ppData, startingNodeName, endingNodeName):
    #a*

    #find data for starting node
    startingNode = findNode(ppData, startingNodeName)

    #list of open ends
    openNodes = []

    #list of node names that have been already closed
    closedNodes = []

    #get node cost
    def costKey(node) -> float:
        return node.m_cost

    for link in startingNode.findall("./pplink"):
        openNodes.append(PPNode(findNode(ppData, link.get("neighbor")), [startingNodeName], float(link.get("cost")), link.get("neighbor")))

    closedNodes.append(startingNode.get("name"))
    
    #loop until end node is found or there are no more open nodes
    endNode = None
    while len(openNodes) > 0:
        #sort nodes by lowest cost
        openNodes.sort(key=costKey)

        #this will always be the lowest cost node
        node = openNodes.pop(0)

        #check to see if end node has been found
        if(node.m_name == endingNodeName):
            endNode = node
            break

        for link in node.m_node.findall("./pplink"):
            neighborName = link.get("neighbor")

            #ensure the node is not in the closed list
            if neighborName not in closedNodes:
                nodeCost = node.m_cost + float(link.get("cost"))

                #check to see if the node is open

                alreadyOpenNode = findNodeIfOpen(openNodes, neighborName)

                if alreadyOpenNode == None:
                    #if node not open, open it
                    openNodes.append(PPNode(findNode(ppData, neighborName), node.m_parentalStructure, nodeCost, neighborName))
                else:
                    #if node open, check cost
                    if(alreadyOpenNode.m_cost > nodeCost):
                        #if open cost higher then new cost, change cost and structure to new                         
                        alreadyOpenNode.m_cost = nodeCost

                        alreadyOpenNode.reroute(node.m_parentalStructure, nodeCost)

 
        closedNodes.append(node.m_name)

    if endNode == None:
        print("No valid path could be found!")
        return

    return endNode.m_parentalStructure

def findNode(nodes, name):
    #find data for starting node
    startingNode = None
    for node in nodes.findall("./ppnode"):
        if(node.get("name") == name):
            startingNode = node

    if startingNode == None:
        print("Starting node cannot be found. Cannot find path!")
        return
    
    return startingNode

def findNodeIfOpen(openNodes, name):
    #find node if the node is already open

    for node in openNodes:
        if(node.m_name == name):
            return node

    return None

def getLowestOpenNode(nodes):
    if len(nodes) == 0:
        print("No nodes!")
        return

    #get the node with the lowest cost
    lowestNode = None
    lowestCost = -1
    for node in nodes:
        if(lowestCost == -1 or node.m_cost < lowestCost):
            lowestCost = node.m_cost
            lowestNode = node

    return lowestNode

if __name__ == "__main__":
    data = loadPPData()
    getRoute(data, "Node1-H", "Node3-F")