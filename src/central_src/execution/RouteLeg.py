class RouteLeg:
    m_step = 0 # the command that take the robot between the starting and ending location
    m_startingLocation = "" # the starting location of the leg
    m_endingLocation = "" # the ending location of the leg 

    def __init__(self, step, startLocation, endLocation):
        self.m_step = step
        self.m_startingLocation = startLocation
        self.m_endingLocation = endLocation
