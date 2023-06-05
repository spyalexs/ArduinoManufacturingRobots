class GUIInMessage:
    #a class for a message incoming to the central controller from the GUI

    #whether or not the message should be sent directly to the bridge or not
    m_direct = True

    def __init__(self, Target, Characteristic, Value, Direct=True):
        #initialization for messages
        self.m_target = Target # the bot to target
        self.m_characteristic = Characteristic # the characteristic on the target to set or type of controller command
        self.m_value = Value # the value to set the characteristic to or controller commmand data

        self.m_direct = Direct

    def getDirectString(self):
        #the string version of the message to be passed by serial to the bridge
        if(self.direct):
            return str(self.m_target) + "$" + str(self.m_characteristic) + "$" + str(self.m_value)

