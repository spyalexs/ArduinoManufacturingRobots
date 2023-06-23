#include <ArduinoBLE.h>
#include "Bot.h"
#include "BotHolder.h"
#include "Connection.h"
#include "MessageLine.h"

BotHolder botHolder(10);
static Connection connections[10];

double connectionTimeout = .5; //seconds

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
  Serial.setTimeout(100);

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

  //begin connecting to devices
  bool connectionMode = true;
  while(connectionMode){
    connectToBot();

    //check serial to see if brige has sent a message to stop
    String string  = Serial.readString();
    if(string == "bridge$connections$stop"){
      connectionMode = false;
    }
  }
}

void loop(){ 
  // //run this loop constantly - it allows for lost connections to be resume
  // //likely need to implement a scan phase and a run phase - scaning takes a while and it does not make sense to do this in loop

  // //comunication with other bots

  //maintain bot connections
  botHolder.cycle();

  //communicate with central
  String string  = Serial.readString();
  string.trim();

  if(string != ""){
    Serial.println("Here - message recieved");

    MessageLine message(string);
    botHolder.sendMessageToBot(message);
  }
}

void connectToBot(){
  //check existing connections
  //connected must be called to update characteristics

  //this is the bot
  BLEDevice bot;

  //Serial.println("Discovering Bot...");
  double timeoutTime = micros() / 1000000.0 + connectionTimeout;

  do{

    //scan, wait then use connect to available
    BLE.scan();
    delay(100);

    //the avialble bot, bots?
    bot = BLE.available();

  }while(!bot && (micros() / 1000000.0 < timeoutTime));

  if(bot){
    
    //if a bot has been fround

    // Serial.print("* Device MAC address: ");
    // Serial.println(bot.address());
    // Serial.print("* Device name: ");
    // Serial.println(bot.localName());

    if(bot.advertisedServiceUuid() != "" && bot.localName() == "TestBot"){
      //if the device is really a bot

      //stop scanning for BLE devices
      BLE.stopScan();

      //establish a connection
      if(bot.connect()){

        //discover characteristics on bot
        if(bot.discoverAttributes()){
          botHolder.appendNewBot(Bot(&bot, botHolder.getBotCount() + 1));

          Serial.println(botHolder.getConnectionsString().c_str()); 
        }
      }
    }
  }
}




