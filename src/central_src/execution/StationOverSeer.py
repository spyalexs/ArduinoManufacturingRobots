from execution.OverSeer import OverSeer

class StationOverSeer(OverSeer):
    #twin to bot overseer 
    
    m_item: str = None

    def __init__(self, macAddress, port, ip_address, queueToBots, queuePacketOut, queueToGUI):
        #specifiy that this is a station overseer
        super().__init__(macAddress, port, ip_address, queueToBots, queuePacketOut, queueToGUI)

        self.m_type = "station"


    def cycle(self):
        #call super cycle to maintain connection

        previousConnectionStatus = self.m_connected

        super().cycle()

        if(previousConnectionStatus == self.m_connected):
            #update GUI if connection status has changed
            self.sendConnectionStatusToGui()

    def setStationItem(self, item: str):
        #set the item the station transfers
        self.m_item = item

        #tell the station to update the item
        self.sendMessageToOverseen(self.m_port + "$setItem$" + item)

    def dispenseItem(self, target):

        #tell station to dispense an item to a certain bot
        self.sendMessageToOverseen(self.m_port + "$dispenseItem$" + target)
