#include <ArduinoBLE.h>
#include <ArduinoMotorCarrier.h>
#include "Command.h"

bool PUBLISHDATA = true;

const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* LEDCUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";
const char* mindControlCUuid = "19b10002-e8f2-537e-4f6c-d104768a1214";
const char* M1CUuid = "19b10003-e8f2-537e-4f6c-d104768a1214";
const char* M2CUuid = "19b10004-e8f2-537e-4f6c-d104768a1214";
const char* E1CUuid = "19b10005-e8f2-537e-4f6c-d104768a1214";
const char* E2CUuid = "19b10006-e8f2-537e-4f6c-d104768a1214";
const char* A1CUuid = "19b10007-e8f2-537e-4f6c-d104768a1214";
const char* A2CUuid = "19b10008-e8f2-537e-4f6c-d104768a1214";
const char* A3CUuid = "19b10009-e8f2-537e-4f6c-d104768a1214";
const char* A4CUuid = "19b10010-e8f2-537e-4f6c-d104768a1214";
const char* commandIssueUuid = "19b10011-e8f2-537e-4f6c-d104768a1214";
const char* commandStatusUuid = "19b10012-e8f2-537e-4f6c-d104768a1214";
const char* batteryVoltageUuid = "19b10011-e8f2-537e-4f6c-d104768a1214";

BLEService motorCarrierService(deviceServiceUuid); 
BLEByteCharacteristic LEDC(LEDCUuid, BLERead | BLEWrite);
BLEByteCharacteristic mindControlC(mindControlCUuid, BLERead | BLEWrite);
BLEByteCharacteristic M1C(M1CUuid, BLERead | BLEWrite);
BLEByteCharacteristic M2C(M2CUuid, BLERead | BLEWrite);
BLEByteCharacteristic E1C(E1CUuid, BLERead | BLEWrite | BLEWriteWithoutResponse);
BLEByteCharacteristic E2C(E2CUuid, BLERead | BLEWrite);
BLEByteCharacteristic A1C(A1CUuid, BLERead | BLEWrite);
BLEByteCharacteristic A2C(A2CUuid, BLERead | BLEWrite);
BLEByteCharacteristic A3C(A3CUuid, BLERead | BLEWrite);
BLEByteCharacteristic A4C(A4CUuid, BLERead | BLEWrite);
BLEByteCharacteristic commandIssueC(commandIssueUuid, BLERead | BLEWrite);
BLEByteCharacteristic commandStatusC(commandStatusUuid, BLERead | BLEWrite);
BLEByteCharacteristic batteryVoltageC(batteryVoltageUuid, BLERead | BLEWrite);

void setup(){
  //start up led
  pinMode(LED_BUILTIN, OUTPUT);

  //start up Bluetooth module
  if(!BLE.begin()){
    Serial.println("Failed to start BLE!");
  }

  //start up motor controller
  if(!controller.begin()){
    Serial.println("Failed to start motor controller!");
  }

  BLE.setLocalName("TestBot");
  BLE.setAdvertisedService(motorCarrierService);

  //initialize all characteristics
  motorCarrierService.addCharacteristic(LEDC);
  motorCarrierService.addCharacteristic(mindControlC);
  motorCarrierService.addCharacteristic(M1C);
  motorCarrierService.addCharacteristic(M2C);
  motorCarrierService.addCharacteristic(E1C);
  motorCarrierService.addCharacteristic(E2C);
  motorCarrierService.addCharacteristic(A1C);
  motorCarrierService.addCharacteristic(A2C);
  motorCarrierService.addCharacteristic(A3C);
  motorCarrierService.addCharacteristic(A4C);
  motorCarrierService.addCharacteristic(commandIssueC);
  motorCarrierService.addCharacteristic(commandStatusC);  
  motorCarrierService.addCharacteristic(batteryVoltageC);
  
  BLE.addService(motorCarrierService);
  
  //apply characteristics default values
  LEDC.writeValue(0);
  mindControlC.writeValue(0);
  M1C.writeValue(0);
  M2C.writeValue(0);  
  E1C.writeValue(0);
  E2C.writeValue(0);  
  A1C.writeValue(0);
  A2C.writeValue(0);  
  A3C.writeValue(0);
  A4C.writeValue(0);
  commandIssueC.writeValue(0);
  commandStatusC.writeValue(0);
  batteryVoltageC.writeValue(0);

  BLE.advertise();

  Serial.println("I am a bot!");
}

void loop(){
    Serial.println("Searching for central...");

    BLEDevice bridge = BLE.central();
    delay(500);

    if(bridge){
      Serial.println("I found central!");
      delay(1000);
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
      //internal led
      setLEDStatus(LEDC.value());
    }

    if(M1C.written()){
      //Motor 1
      //setM1(M1C.value());
    }

    if(M2C.written()){
      //motor 2
      setM2(M2C.value());
    }

    E1C.writeValue(getE2());
    E2C.writeValue(getE2());
    batteryVoltageC.writeValue(getBatteryVoltage());

    delay(1000);
  } else if(commandIssueC.value() != 0) {
    int commandVal = commandIssueC.value();
    commandIssueC.writeValue(0);

    switch (commandVal){
      case 1:
        break;
      case 2:
        break;
      default:
        break;
    }
  }

  Command test(&A1C);
}

void setLEDStatus(int status){
  if(status == 0){
    digitalWrite(LED_BUILTIN, LOW);
  }else{
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

void setM1(int duty){
  //set motor 1 duty
  M1.setDuty(duty);
}

void setM2(int duty){
  //set motor 2 duty
  M2.setDuty(duty);
}

int getE1(){
  //get the value of encoder 1
  Serial.println(encoder1.getRawCount());
  return encoder1.getRawCount();
}

int getE2(){
  //get the value of encoder 2
  return encoder1.getRawCount();
}

int resetE1(){
  //reset encoder 1
  encoder1.resetCounter(0);
  return getE1();
}

int resetE2(){
  //reset encoder 2
  encoder2.resetCounter(0);
  return getE2();
}

float getBatteryVoltage(){
  //return the battery voltage
  //must adjust in the central processor / divide by 236
  return battery.getRaw();
}