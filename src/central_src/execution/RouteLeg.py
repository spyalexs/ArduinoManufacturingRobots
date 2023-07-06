class RouteLeg:
    m_step = 0 # the command that take the robot between the starting and ending location
    m_startingLocation = "" # the starting location of the leg
    m_endingLocation = "" # the ending location of the leg 
    m_progressLocation = "" # the location in the middle of the leg

    def __init__(self, step, startLocation, endLocation):
        self.m_step = step
        self.m_startingLocation = startLocation
        self.m_endingLocation = endLocation

        #if just a wrapper, quit
        if(self.m_startingLocation == None or self.m_endingLocation == None):
            self.m_progressLocation = None
            return

        #determine the middle location
        if("Node" in self.m_startingLocation and "Node" in self.m_endingLocation):
            #either a node to node connection or a intranode connection
            
            #determine if intranode
            startNode = self.m_startingLocation.split("-")
            endNode = self.m_endingLocation.split("-")

            if(startNode[0] == endNode[0]):
                #set progress location to the middle of the node
                self.m_progressLocation = startNode[0]
            else:
                self.m_progressLocation = self.m_startingLocation + "to" + self.m_endingLocation
            
        else:
            #connection involves action points
            print("Not Ready Yet")




