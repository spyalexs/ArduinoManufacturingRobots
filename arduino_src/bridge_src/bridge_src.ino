#include <ArduinoBLE.h>

const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* deviceServiceCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";


void setup(){
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
  

  connectToBot();
}

void connectToBot(){
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
      writeToBot(bot);
    }
  }
}

void writeToBot(BLEDevice bot){
  while(bot.connected()){
    Serial.println("Connected");
    //get charcteristic from bot

    Serial.println(bot.discoverAttributes());

    BLECharacteristic botCharacteristic = bot.characteristic(deviceServiceCharacteristicUuid);

    Serial.println(botCharacteristic);

    botCharacteristic.writeValue((byte)12);
    delay(1000);
  }
}