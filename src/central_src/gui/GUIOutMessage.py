class GUIOutMessage:
    # a class for messages from the controller to the GUI

    def __init__(self, Target, Characteristic, Value):
        # the initializer for messages from a bot

        self.m_target = Target # the bot the raised the message
        self.m_characteristic = Characteristic # the characteristic that the message is based on
        self.m_value = Value # the value passed 

