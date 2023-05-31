class GUIInMessage:
    #a class for a message incoming to the central controller from the GUI

    #whether or not the message should be sent directly to the bridge or not
    direct = True

    def __init__(self, Target, Characteristic, Value):
        #initialization for direct messages
        self.target = Target # the bot to target
        self.characteristic = Characteristic # the characteristic on the target to set
        self.value = Value # the value to set the characteristic to 

        self.direct = True

    def getDirectString(self):
        #the string version of the message to be passed by serial to the bridge
        if(self.direct):
            return str(self.target) + "$" + str(self.characteristic) + "$" + str(self.value)

