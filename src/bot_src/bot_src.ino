//libs
#include <ArduinoBLE.h>
#include <ArduinoMotorCarrier.h>

#include "RobotContainer.h"

//commands
#include "TurnRight.h"
#include "TurnLeft.h"
#include "FollowLineUntilMarker.h"
#include "TravelStraight.h"
#include "FollowLineOnMarker.h"

bool PUBLISHDATA = true;

//the robot container that is a wrapper around periphral functions
RobotContainer MC = RobotContainer(&M1, &M2, &encoder1, &encoder2, A3, A6, A2);;

//Uuid for connection service
const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";

//Uuid for characteristics
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
const char* batteryVoltageUuid = "19b10011-e8f2-537e-4f6c-d104768a1214";
const char* commandStatusUuid = "19b10012-e8f2-537e-4f6c-d104768a1214";
const char* commandIssueUuid = "19b10013-e8f2-537e-4f6c-d104768a1214";

//Connection Services
BLEService motorCarrierService(deviceServiceUuid); 

//Characteristics
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
  //initial startup for bot

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

  //set local name (bluetooth) display name and advertise service
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

  //advertise the bot on the bluetooth network
  BLE.advertise();

  Serial.println("I am a bot!");
}

void loop(){
    Serial.println("Searching for central...");

    //this performs a scan for the bridge
    BLEDevice bridge = BLE.central();
    delay(500);

    if(bridge){
      Serial.println("I found central!");

      //add bridge to MC
      MC.setCentralPtr(&bridge);

      delay(1000);
      while(bridge.connected()){
        //run bot functions while in connected phase
        connectedLoop(bridge);
      }

      //need to take a look at this - even when definetly disconnected, the bot never seems to register, although bridge easily reconnects
      Serial.println("Disconnected from the bridge! - Stopping until connection reaquirred.");
    }
}

void connectedLoop(BLEDevice testd){
//this runs while the bot is connected to the bridge

  if(mindControlC.value()){
    //if the robot is in mind control mode - run simple commands

    if(LEDC.written()){
      //internal led
      MC.setLEDStatus(LEDC.value());
    }

    if(M1C.written()){
      //Motor 1
      MC.setMotor1(M1C.value());
    }

    if(M2C.written()){
      //motor 2
      MC.setMotor2(M2C.value());
    }

    //write back encoder values
    E1C.writeValue(MC.getEncoder1Counts());
    E2C.writeValue(MC.getEncoder2Counts());

    //write back the integer battery voltage which in the voltage * 236
    batteryVoltageC.writeValue(MC.getBatteryVoltage() * 236);

    delay(1000);

  } else if(commandIssueC.value() != 0) {
    //if the robot is in command mode - normal operation 

    //get which command and then write over it to over doing the command multiple times unnessecarily
    int commandVal = commandIssueC.value();
    commandIssueC.writeValue(byte(0));

    //run commands
    //0 - no command, shouldn't be in this switch anyways
    //1 - testing
    //2 to 99 - base actions
    //101 to 199 - compound actions
    //255 - abort!
    switch (commandVal){
      case 1:
        Serial.println("Testing!");
        break;

      case 2:
        if(true){
          FollowLineUntilMarker commandToRun(&commandStatusC, &commandIssueC, &MC);
          commandToRun.run();
        }
        break;

      case 3:
        if(true){
          FollowLineOnMarker commandToRun(&commandStatusC, &commandIssueC, &MC);
          commandToRun.run();
        }
        break;

      case 4:
        if(true){
          TravelStraight commandToRun(&commandStatusC, &commandIssueC, &MC, 210);
          commandToRun.run();
        }
        break;

      case 5:
        if(true){
          TurnRight commandToRun(&commandStatusC, &commandIssueC, &MC);
          commandToRun.run();
        }
        break;

      case 6:
        if(true){
          TurnLeft commandToRun(&commandStatusC, &commandIssueC, &MC);
          commandToRun.run();
        }
        break;

      case 101:
        //full right turn through intersection
        if(true){
          FollowLineOnMarker commandToRun(&commandStatusC, &commandIssueC, &MC);
          commandToRun.run();
          TravelStraight commandToRun2(&commandStatusC, &commandIssueC, &MC, 210);
          commandToRun2.run();
          TurnRight commandToRun3(&commandStatusC, &commandIssueC, &MC);
          commandToRun3.run();
        }
        break;

      case 102:
        //full straight through intersection
        if(true){
          FollowLineOnMarker commandToRun(&commandStatusC, &commandIssueC, &MC);
          commandToRun.run();
          TravelStraight commandToRun2(&commandStatusC, &commandIssueC, &MC, 200);
          commandToRun2.run();
          FollowLineUntilMarker commandToRun3(&commandStatusC, &commandIssueC, &MC);
          commandToRun3.run();
          TravelStraight commandToRun4(&commandStatusC, &commandIssueC, &MC, 100);
          commandToRun4.run();

        }
        break;

      case 103:
        //full left through intersection
        if(true){
          FollowLineOnMarker commandToRun(&commandStatusC, &commandIssueC, &MC);
          commandToRun.run();
          TravelStraight commandToRun2(&commandStatusC, &commandIssueC, &MC, 200);
          commandToRun2.run();
          FollowLineUntilMarker commandToRun3(&commandStatusC, &commandIssueC, &MC);
          commandToRun3.run();
          TravelStraight commandToRun4(&commandStatusC, &commandIssueC, &MC, 310);
          commandToRun4.run();
          TurnLeft commandToRun5(&commandStatusC, &commandIssueC, &MC);
          commandToRun5.run();
          TravelStraight commandToRun6(&commandStatusC, &commandIssueC, &MC, 40);
          commandToRun6.run();
        }
        break;

      default:
        break;
    }
  }
}