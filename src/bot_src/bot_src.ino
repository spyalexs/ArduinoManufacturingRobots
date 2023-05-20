#include <ArduinoBLE.h>

const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* deviceServiceCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";

BLEService testService(deviceServiceUuid); 
BLEByteCharacteristic testCharacteristic(deviceServiceCharacteristicUuid, BLERead | BLEWrite);


void setup(){
  //start up led
  pinMode(LED_BUILTIN, OUTPUT);

  //start up Bluetooth module
  if(!BLE.begin()){
    Serial.println("Failed to start BLE!");
  }

  BLE.setLocalName("TestBot");
  BLE.setAdvertisedService(testService);
  testService.addCharacteristic(testCharacteristic);
  BLE.addService(testService);
  testCharacteristic.writeValue(0);
  BLE.advertise();

  Serial.println("I am a bot!");
}

void loop(){
    Serial.println("Searching for central...");

    BLEDevice bridge = BLE.central();
    delay(500);

    if(bridge){
      Serial.println("I found central!");

      while(bridge.connected()){
        connectedLoop();
      }

      Serial.println("Disconnected from the bridge! - Stopping until connection reaquirred.");
    }
}

void connectedLoop(){
//this runs while the bot is connected to the bridge
  if(testCharacteristic.written()){
    setLEDStatus(testCharacteristic.value());
  }

  
}


void setLEDStatus(int status){
  if(status == 0){
    digitalWrite(LED_BUILTIN, LOW);
  }else{
    digitalWrite(LED_BUILTIN, HIGH);
  }
}