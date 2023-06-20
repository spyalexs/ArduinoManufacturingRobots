#include <ArduinoBLE.h>
#include "Bot.h"
#include "BotHolder.h"
#include "Connection.h"

BotHolder botHolder(10);
static Connection connections[10];

class MessageLine{
  //class to hold data gotten from the serial port

  public:
    MessageLine(String message){
      //parse raw message to a target characterisitc and value
      int firstSeperator = message.indexOf("$");

      //the target of the message
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


static void BLEDisconnectHandler(BLEDevice bot){
  // a disconnection has been detected
  std::string address = bot.address().c_str();

  Serial.print("Disconnection from bot at address: ");
  Serial.println(String(address.c_str()));

  Serial.println("Odd things may begin to happen: Please reboot the system...");
  //set the bot as disconnected to ensure the entire system does not collapse
  for(int i = 0; i < sizeof(connections) / sizeof(connections[0]); i++){
    if(connections[i].m_address == address){
      connections[i].m_isConnected = false;

      Serial.println("Disconnection Found");
    }
  }
}

static void BLEConnectHandler(BLEDevice bot){
  //a connection has been detected
  
  Serial.print("Connection from bot at address: ");
  Serial.println(bot.address());
}

void setup(){
  //set baud rate
  Serial.begin(38400);

  //start up bluetooth module
  if(!BLE.begin()){
    Serial.println("Failed to start BLE!");
  }

  BLE.setEventHandler(BLEDisconnected, BLEDisconnectHandler);
  BLE.setEventHandler(BLEConnected, BLEConnectHandler);

  delay(500);

  //advertise self
  BLE.setLocalName("Nano 33 BLE (Central)");
  BLE.advertise();

  //attach connection array to the bot holder
  botHolder.setConnectionsPtr(connections);

  Serial.println("I am the bridge!");
}

void loop(){ 
  // //run this loop constantly - it allows for lost connections to be resume
  // //likely need to implement a scan phase and a run phase - scaning takes a while and it does not make sense to do this in loop

  // //comunication with other bots
  while(botHolder.getBotCount() < 2){
    connectToBot();
  }

  //maintain bot connections
  botHolder.cycle();
}

void connectToBot(){
  //check existing connections
  //connected must be called to update characteristics

  //this is the bot
  BLEDevice bot;

  Serial.println("Discovering Bot...");

  do{

    //scan, wait then use connect to available
    BLE.scan();
    delay(100);

    //the avialble bot, bots?
    bot = BLE.available();

  }while(!bot);

  if(bot){
    
    //if a bot has been fround
    Serial.println("I found a device!");

    Serial.print("* Device MAC address: ");
    Serial.println(bot.address());
    Serial.print("* Device name: ");
    Serial.println(bot.localName());

    if(bot.advertisedServiceUuid() != "" && bot.localName() == "TestBot"){
      //if the device is really a bot

      //stop scanning for BLE devices
      BLE.stopScan();

      //establish a connection
      if(bot.connect()){

        //discover characteristics on bot
        if(bot.discoverAttributes()){
          botHolder.appendNewBot(Bot(&bot, 1));

          Serial.println(botHolder.getConnectionsString().c_str()); 
        }
      }
    }
  }
}




