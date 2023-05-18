#include <ArduinoBLE.h>

const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* deviceServiceCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";

BLEService testService(deviceServiceUuid); 
BLEByteCharacteristic testCharacteristic(deviceServiceCharacteristicUuid, BLERead | BLEWrite);


void setup(){
  if(!BLE.begin()){
    Serial.println("Failed to start BLE!");
  }

  BLE.setLocalName("TestBot");
  BLE.setAdvertisedService(testService);
  testService.addCharacteristic(testCharacteristic);
  BLE.addService(testService);
  testCharacteristic.writeValue(-1);
  BLE.advertise();

  Serial.println("I am a bot!");
}

void loop(){
    Serial.print("Characteristic Value: ");
    Serial.println(testCharacteristic.value());

    BLEDevice centralCommand = BLE.central();
    delay(500);

    if(centralCommand){
      Serial.println("I found central!");

      while(centralCommand.connected()){
        if(testCharacteristic.written()){
          Serial.print("Characteristic Value: ");
          Serial.println(testCharacteristic.value());
        }

        delay(100);
      }

      Serial.println("Disconnected from central command!");
    }

}