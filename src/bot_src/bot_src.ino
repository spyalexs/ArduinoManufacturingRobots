#include <ArduinoBLE.h>
#include <ArduinoMotorCarrier.h>

const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* LEDCUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";
const char* mindControlCUuid = "19b10002-e8f2-537e-4f6c-d104768a1214";
const char* M1CUuid = "19b10003-e8f2-537e-4f6c-d104768a1214";
const char* M2CUuid = "19b10004-e8f2-537e-4f6c-d104768a1214";

BLEService testService(deviceServiceUuid); 
BLEByteCharacteristic LEDC(LEDCUuid, BLERead | BLEWrite);
BLEByteCharacteristic mindControlC(mindControlCUuid, BLERead | BLEWrite);
BLEByteCharacteristic M1C(M1CUuid, BLERead | BLEWrite);
BLEByteCharacteristic M2C(M2CUuid, BLERead | BLEWrite);


void setup(){
  //start up led
  pinMode(LED_BUILTIN, OUTPUT);

  //start up Bluetooth module
  if(!BLE.begin()){
    Serial.println("Failed to start BLE!");
  }

  BLE.setLocalName("TestBot");
  BLE.setAdvertisedService(testService);
  testService.addCharacteristic(LEDC);
  testService.addCharacteristic(mindControlC);
  testService.addCharacteristic(M1C);
  testService.addCharacteristic(M2C);
  BLE.addService(testService);
  
  //apply characteristics default values
  LEDC.writeValue(0);
  mindControlC.writeValue(0);
  M1C.writeValue(0);
  M2C.writeValue(0);

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

  if(mindControlC.value()){
    //if the robot is in mind control mode - run simple commands

    if(LEDC.written()){
      setLEDStatus(LEDC.value());
    }

    if(M1C.written()){
      setM1(M1C.value());
    }

    if(M2C.written()){

    }

  }

  Serial.println(mindControlC.value());
}


void setLEDStatus(int status){
  if(status == 0){
    digitalWrite(LED_BUILTIN, LOW);
  }else{
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

void setM1(int duty){

}

void setM2(int duty){

}