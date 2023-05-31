#include <ArduinoBLE.h>

const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";

//Uuids for bot 1
const char* bot1_LEDCUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";
const char* bot1_mindControlCUuid = "19b10002-e8f2-537e-4f6c-d104768a1214";
const char* bot1_M1CUuid = "19b10003-e8f2-537e-4f6c-d104768a1214";
const char* bot1_M2CUuid = "19b10004-e8f2-537e-4f6c-d104768a1214";
const char* bot1_E1CUuid = "19b10005-e8f2-537e-4f6c-d104768a1214";
const char* bot1_E2CUuid = "19b10006-e8f2-537e-4f6c-d104768a1214";
const char* bot1_A1CUuid = "19b10007-e8f2-537e-4f6c-d104768a1214";
const char* bot1_A2CUuid = "19b10008-e8f2-537e-4f6c-d104768a1214";
const char* bot1_A3CUuid = "19b10009-e8f2-537e-4f6c-d104768a1214";
const char* bot1_A4CUuid = "19b10010-e8f2-537e-4f6c-d104768a1214";
const char* bot1_batteryVoltageUuid = "19b10011-e8f2-537e-4f6c-d104768a1214";
const char* bot1_statusCUuid = "19b10012-e8f2-537e-4f6c-d104768a1214";
const char* bot1_issueCUuid = "19b10013-e8f2-537e-4f6c-d104768a1214";

BLEDevice bot1;
BLECharacteristic bot1_LEDC;
BLECharacteristic bot1_mindControlC;
BLECharacteristic bot1_M1C;
BLECharacteristic bot1_M2C;
BLECharacteristic bot1_E1C;
BLECharacteristic bot1_E2C;
BLECharacteristic bot1_A1C;
BLECharacteristic bot1_A2C;
BLECharacteristic bot1_A3C;
BLECharacteristic bot1_A4C;
BLECharacteristic bot1_batteryVoltageC;
BLECharacteristic bot1_issueC;
BLECharacteristic bot1_statusC;

int bot1_statusCStoredValue = 0;


bool bot1_mindControl = false;//whether the bot is in mindcontrol state or not

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

      //Serial.println("$$$$$" + m_characteristic);
    }

    MessageLine(String target, String characteristic, String value){
      m_target = target;
      m_characteristic = characteristic;
      m_value = value;
    }

    String getSerialString(){
      return "$$$$$" + m_target + "$" + m_characteristic + "$" + m_value;
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
  //communicate with bots
  communicateWithBots();
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
        bot1_LEDC = bot.characteristic(bot1_LEDCUuid);
        bot1_mindControlC = bot.characteristic(bot1_mindControlCUuid);
        bot1_M1C = bot.characteristic(bot1_M1CUuid);
        bot1_M2C = bot.characteristic(bot1_M2CUuid);
        bot1_E1C = bot.characteristic(bot1_E1CUuid);
        bot1_E2C = bot.characteristic(bot1_E2CUuid);
        bot1_A1C = bot.characteristic(bot1_A1CUuid);
        bot1_A2C = bot.characteristic(bot1_A2CUuid);
        bot1_A3C = bot.characteristic(bot1_A3CUuid);
        bot1_A4C = bot.characteristic(bot1_A4CUuid);
        bot1_batteryVoltageC = bot.characteristic(bot1_batteryVoltageUuid);
        bot1_issueC = bot.characteristic(bot1_issueCUuid);
        bot1_statusC = bot.characteristic(bot1_statusCUuid);

        bot1 = bot;
      }
    }
  }
}

void communicateWithBots(){
  //take all bot messages and send to central

 
  delay(100);

  if(bot1_mindControl){
    //pass back basic status memos
    byte value = 0 ;
    bot1_E1C.readValue(value);
    MessageLine message("bot1", "E1", String(int(value)));
    Serial.println(message.getSerialString());

    bot1_E2C.readValue(value);
    message = MessageLine("bot1", "E2", String(int(value)));
    Serial.println(message.getSerialString());

    // bot1_A1C.readValue(value);
    // MessageLine message("bot1", "A1", String(int(value)));
    // Serial.println(message.getSerialString());

    // bot1_A2C.readValue(value);
    // MessageLine message("bot1", "A2", String(int(value)));
    // Serial.println(message.getSerialString());

    // bot1_A3C.readValue(value);
    // MessageLine message("bot1", "A3", String(int(value)));
    // Serial.println(message.getSerialString());

    // bot1_A4C.readValue(value);
    // MessageLine message("bot1", "A4", String(int(value)));
    // Serial.println(message.getSerialString());

    bot1_batteryVoltageC.readValue(value);
    message = MessageLine("bot1", "batteryVoltage", String(int(value)));
    Serial.println(message.getSerialString());
  }else{
    byte value = 0;
    bot1_statusC.readValue(value);
    if(bot1_statusCStoredValue != int(value)){
      bot1_statusCStoredValue = int(value);
      MessageLine message("bot1", "commandStatus", String(int(value)));
      Serial.println(message.getSerialString());
    }
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
      if(bot1_LEDC){
        bot1_LEDC.writeValue(byte(line.m_value.toInt()));
      }
    }else if(line.m_characteristic == "mindControl"){
      if(bot1_mindControlC){
        bot1_mindControlC.writeValue(byte(line.m_value.toInt()));

        //update mind control state to determine which information to send back to central
        if(line.m_value.toInt()){
          Serial.println("MindControl-----------------------------------------");
          bot1_mindControl = true;
        }else{
          Serial.println("MindControl Off-------------------------------------------------");
          bot1_mindControl = false;
        }

      }
    }else if(line.m_characteristic == "M1"){
      if(bot1_M1C){
        bot1_M1C.writeValue(byte(line.m_value.toInt()));
      }
    }else if(line.m_characteristic == "M2"){
      if(bot1_M2C){
        bot1_M2C.writeValue(byte(line.m_value.toInt()));
      }
    }else if(line.m_characteristic == "commandIssue"){
      if(bot1_issueC){
        bot1_issueC.writeValue(byte(line.m_value.toInt()));
      }
    }
  }
}

