from gui.GUIOutMessage import GUIOutMessage

def handleBotMessage(message, queueInGUI, overseers):
    # handle messages coming in from the bridge

    #messages should be string in from serial
    messageArray = message.split('$')
    
    if(len(messageArray) != 3):
        #check format of message
        print("Message corrupted, failed to handle: " + str(message))
        return
    
    target = messageArray[0]# the bot that sent in the message
    characteristic = messageArray[1] # the characteristic the message is about
    value = messageArray[2] # the value of the message

    if(characteristic == "bat"):
        #messages concerning battery voltage
        queueInGUI.put(GUIOutMessage(target, characteristic, float(value)))

    if(characteristic == "commandStatus"):
        #message concerning command status

        # find overseer
        for overseer in overseers:
            if (overseer.m_port == target):
                overseer.updateStatus(int(value))

    if(characteristic == "iconRequest"):
        for overseer in overseers:
            if (overseer.m_port == target):
                overseer.sendPacket(value) 

    #log connection so the bot overseer know the bot is still connected
    for overseer in overseers:
        if (overseer.m_port == target):
            overseer.connectionHeard()



