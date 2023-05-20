#include <ArduinoBLE.h>

const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* deviceBUILTINLEDCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";

BLEDevice bot1;
BLECharacteristic bot1_BUILTIN_LED;


class MessageLine{
  //class to hold data gotten from the serial port

  public:
    MessageLine(String message){
      //parse raw message to a target characterisitc and value
      int firstSeperator = message.indexOf("$");

      m_target = message.substring(0, firstSeperator);
      String backString = message.substring(firstSeperator + 1);
      int secondSeperator = backString.indexOf("$");

      m_characteristic = backString.substring(0, secondSeperator);
      m_value = backString.substring(secondSeperator + 1);

      Serial.println("$$$$$" + m_characteristic);
    }

    String m_target;
    String m_characteristic;
    String m_value;
};

void setup(){
  //set baud rate
  Serial.begin(38400);

  //start up bluetooth module
  if(!BLE.begin()){
    Serial.println("Failed to start BLE!");
  }

  delay(500);

  //advertise self
  BLE.setLocalName("Nano 33 BLE (Central)");
  BLE.advertise();

  Serial.println("I am the bridge!");
}

void loop(){  
  //comunication with other bots
  connectToBot();
  //communication with central
  communicateWithCentral();
}

void connectToBot(){
  //check existing connections
  if(bot1.connected()){
    return;
  }

  //this is the bot
  BLEDevice bot;

  Serial.println("Discovering Bot...");

  do{
    Serial.println("Looking for bot!");

    //scan, wait then use connect to available
    BLE.scanForUuid(deviceServiceUuid);
    delay(1000);
    bot = BLE.available();

  }while(!bot);

  if(bot){
    //if a bot has been fround
    Serial.println("I found a bot!");

    Serial.print("* Device MAC address: ");
    Serial.println(bot.address());
    Serial.print("* Device name: ");
    Serial.println(bot.localName());
    Serial.print("* Advertised service UUID: ");
    Serial.println(bot.advertisedServiceUuid());

    BLE.stopScan();

    if(bot.connect()){
      Serial.println("Connected to bot1!");
      
      //discover characteristics on bot
      if(bot.discoverAttributes()){
        Serial.println("Atrributes found on bot1!");

        //save characteristics
        bot1_BUILTIN_LED = bot.characteristic(deviceBUILTINLEDCharacteristicUuid);

        
        bot1 = bot;
      }
    }
  }
}

void writeToBot(BLEDevice bot){
  while(bot.connected()){
    Serial.println("Connected");
    //get charcteristic from bot

    Serial.println(bot.discoverAttributes());

    BLECharacteristic botCharacteristic = bot.characteristic(deviceBUILTINLEDCharacteristicUuid);

    Serial.println(botCharacteristic);

    botCharacteristic.writeValue((byte)12);
    delay(1000);
  }
}

void communicateWithCentral(){
  String message = Serial.readString();
  Serial.setTimeout(10);

  message.trim();
  if(!(message == "")){
    //Serial.println(message);

    processMessage(message);
  }
}

void processMessage(String message){
  //Message Line Format
  //TARGETBOT$CHARACTERISTIC$VALUE
  MessageLine line = MessageLine(message);

  writeLine(line);
}

void writeLine(MessageLine line){
  BLEDevice target;

  //match up target and characteristic
  if(line.m_target == "bot1"){
    if(line.m_characteristic == "BUILTIN_LED"){
      if(bot1_BUILTIN_LED){
        bot1_BUILTIN_LED.writeValue(byte(line.m_value.toInt()));
      }
    }
  }
}

