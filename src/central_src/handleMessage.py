from gui.GUIOutMessage import GUIOutMessage

def handleBotMessage(message, queueInGUI):
    # handle messages coming in from the bridge

    #messages should be string in from serial
    messageArray = message.split('$')
    
    if(len(messageArray) != 3):
        #check format of message
        print("Message corrupted, failed to handle: " + str(message))
        return
    
    target = messageArray[0]# the bot that sent in the message
    characteristic = messageArray[1] # the characteristic the message is about
    value = int(messageArray[2]) # the value of the message

    if(characteristic == "E1"):
        #messages concerning encoder 1
        queueInGUI.put(GUIOutMessage(target, characteristic, value))

    if(characteristic == "E2"):
        #messages concerning encoder 2
        queueInGUI.put(GUIOutMessage(target, characteristic, value))

    if(characteristic == "A1"):
        #messages concerning analog input 1
        queueInGUI.put(GUIOutMessage(target, characteristic, value))

    if(characteristic == "A2"):
        #messages concerning analog input 2
        queueInGUI.put(GUIOutMessage(target, characteristic, value))

    if(characteristic == "A3"):
        #messages concerning analog input 3
        queueInGUI.put(GUIOutMessage(target, characteristic, value))

    if(characteristic == "A4"):
        #messages concerning analog input 4
        queueInGUI.put(GUIOutMessage(target, characteristic, value))

    if(characteristic == "batteryVoltage"):
        #messages concerning battery voltage
        queueInGUI.put(GUIOutMessage(target, characteristic, value))

    if(characteristic == "status"):
        #message concerning command status



