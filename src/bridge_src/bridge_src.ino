#include <ArduinoBLE.h>
#include "Bot.h"
#include "BotHolder.h"

BotHolder botHolder(10);

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
  // //run this loop constantly - it allows for lost connections to be resume
  // //likely need to implement a scan phase and a run phase - scaning takes a while and it does not make sense to do this in loop

  // //comunication with other bots
  if(botHolder.getBotCount() < 1){
    connectToBot();
  }
  // //communicate with bots
  // communicateWithBots();
  // //communication with central
  // communicateWithCentral();
}

void connectToBot(){
  //check existing connections
  //connected must be called to update characteristics

  //this is the bot
  BLEDevice bot;

  Serial.println("Discovering Bot...");

  do{
    Serial.println("Looking for bot!");

    //scan, wait then use connect to available
    BLE.scan();
    delay(1000);

    //the avialble bot, bots?
    bot = BLE.available();

  }while(!bot);

  if(bot){
    //if a bot has been fround
    Serial.println("I found a bot!");

    Serial.print("* Device MAC address: ");
    Serial.println(bot.address());
    Serial.print("* Device name: ");
    Serial.println(bot.localName());

    if(bot.advertisedServiceUuid() != ""){
      //if the device is really a bot

      //stop scanning for BLE devices
      BLE.stopScan();

      //establish a connection
      if(bot.connect()){
        Serial.println("Connected to bot1!");
        
        //discover characteristics on bot
        if(bot.discoverAttributes()){
          Serial.println("Atrributes found on bot1!");

          botHolder.appendNewBot(Bot(&bot, 1));

        }
      }
    }
  }
}




