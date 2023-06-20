//libs
#include <ArduinoBLE.h>
#include <ArduinoMotorCarrier.h>
#include <string>

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

//initialize characteristics as dummies ---------------------- fill in later
std::string* serviceUUID = (std::string*)malloc(sizeof(std::string));

//Uuid for characteristics
const char* LEDCUuid = "00000000-0000-0000-0000-000000000000";
const char* mindControlCUuid = "00000000-0000-0000-0000-000000000000"; 
const char* M1CUuid = "00000000-0000-0000-0000-000000000000";
const char* M2CUuid = "00000000-0000-0000-0000-000000000000";
const char* E1CUuid = "00000000-0000-0000-0000-000000000000";
const char* E2CUuid = "00000000-0000-0000-0000-000000000000";
const char* A1CUuid = "00000000-0000-0000-0000-000000000000";
const char* A2CUuid = "00000000-0000-0000-0000-000000000000";
const char* A3CUuid = "00000000-0000-0000-0000-000000000000";
const char* A4CUuid = "00000000-0000-0000-0000-000000000000";
const char* batteryVoltageUuid = "00000000-0000-0000-0000-000000000000";
const char* commandStatusUuid = "00000000-0000-0000-0000-000000000000";
const char* commandIssueUuid = "00000000-0000-0000-0000-000000000000";

//Connection Services

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

//------------------------
 
void setup(){
  delay(5000);

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

  //Uuid for connection service
  std::string deviceServiceUUID = generateUUID(BLE.address().c_str(), "0000");

  //Uuid for characteristics
  std::string LEDCUuidString = generateUUID(BLE.address().c_str(), "0001").c_str();
  const char* LEDCUuid = new const char[36]{LEDCUuidString[0], LEDCUuidString[1], LEDCUuidString[2], LEDCUuidString[3], LEDCUuidString[4], LEDCUuidString[5], LEDCUuidString[6], LEDCUuidString[7], 
    '-', LEDCUuidString[9], LEDCUuidString[10], LEDCUuidString[11], LEDCUuidString[12], 
    '-', LEDCUuidString[14], LEDCUuidString[15], LEDCUuidString[16], LEDCUuidString[17],
    '-', LEDCUuidString[19], LEDCUuidString[20], LEDCUuidString[21], LEDCUuidString[22], 
    '-', LEDCUuidString[24], LEDCUuidString[25], LEDCUuidString[26], LEDCUuidString[27], LEDCUuidString[28], LEDCUuidString[29], LEDCUuidString[30], LEDCUuidString[31], LEDCUuidString[32], LEDCUuidString[33], LEDCUuidString[34], LEDCUuidString[35]};
  *((char *)LEDCUuid + 36) = '\0';

  std::string mindControlCUuidString = generateUUID(BLE.address().c_str(), "0002").c_str();
  const char* mindControlCUuid = new const char[36]{mindControlCUuidString[0], mindControlCUuidString[1], mindControlCUuidString[2], mindControlCUuidString[3], mindControlCUuidString[4], mindControlCUuidString[5], mindControlCUuidString[6], mindControlCUuidString[7], 
    '-', mindControlCUuidString[9], mindControlCUuidString[10], mindControlCUuidString[11], mindControlCUuidString[12], 
    '-', mindControlCUuidString[14], mindControlCUuidString[15], mindControlCUuidString[16], mindControlCUuidString[17],
    '-', mindControlCUuidString[19], mindControlCUuidString[20], mindControlCUuidString[21], mindControlCUuidString[22], 
    '-', mindControlCUuidString[24], mindControlCUuidString[25], mindControlCUuidString[26], mindControlCUuidString[27], mindControlCUuidString[28], mindControlCUuidString[29], mindControlCUuidString[30], mindControlCUuidString[31], mindControlCUuidString[32], mindControlCUuidString[33], mindControlCUuidString[34], mindControlCUuidString[35]};
  *((char *)mindControlCUuid + 36) = '\0';

  std::string M1CUuidString = generateUUID(BLE.address().c_str(), "0003").c_str();
  const char* M1CUuid = new const char[36]{M1CUuidString[0], M1CUuidString[1], M1CUuidString[2], M1CUuidString[3], M1CUuidString[4], M1CUuidString[5], M1CUuidString[6], M1CUuidString[7], 
    '-', M1CUuidString[9], M1CUuidString[10], M1CUuidString[11], M1CUuidString[12], 
    '-', M1CUuidString[14], M1CUuidString[15], M1CUuidString[16], M1CUuidString[17],
    '-', M1CUuidString[19], M1CUuidString[20], M1CUuidString[21], M1CUuidString[22], 
    '-', M1CUuidString[24], M1CUuidString[25], M1CUuidString[26], M1CUuidString[27], M1CUuidString[28], M1CUuidString[29], M1CUuidString[30], M1CUuidString[31], M1CUuidString[32], M1CUuidString[33], M1CUuidString[34], M1CUuidString[35]};
  *((char *)M1CUuid + 36) = '\0';

  std::string M2CUuidString = generateUUID(BLE.address().c_str(), "0004").c_str();
  const char* M2CUuid = new const char[36]{M2CUuidString[0], M2CUuidString[1], M2CUuidString[2], M2CUuidString[3], M2CUuidString[4], M2CUuidString[5], M2CUuidString[6], M2CUuidString[7], 
    '-', M2CUuidString[9], M2CUuidString[10], M2CUuidString[11], M2CUuidString[12], 
    '-', M2CUuidString[14], M2CUuidString[15], M2CUuidString[16], M2CUuidString[17],
    '-', M2CUuidString[19], M2CUuidString[20], M2CUuidString[21], M2CUuidString[22], 
    '-', M2CUuidString[24], M2CUuidString[25], M2CUuidString[26], M2CUuidString[27], M2CUuidString[28], M2CUuidString[29], M2CUuidString[30], M2CUuidString[31], M2CUuidString[32], M2CUuidString[33], M2CUuidString[34], M2CUuidString[35]};
  *((char *)M2CUuid + 36) = '\0';

  std::string E1CUuidString = generateUUID(BLE.address().c_str(), "0005").c_str();  
  const char* E1CUuid = new const char[36]{E1CUuidString[0], E1CUuidString[1], E1CUuidString[2], E1CUuidString[3], E1CUuidString[4], E1CUuidString[5], E1CUuidString[6], E1CUuidString[7], 
    '-', E1CUuidString[9], E1CUuidString[10], E1CUuidString[11], E1CUuidString[12], 
    '-', E1CUuidString[14], E1CUuidString[15], E1CUuidString[16], E1CUuidString[17],
    '-', E1CUuidString[19], E1CUuidString[20], E1CUuidString[21], E1CUuidString[22], 
    '-', E1CUuidString[24], E1CUuidString[25], E1CUuidString[26], E1CUuidString[27], E1CUuidString[28], E1CUuidString[29], E1CUuidString[30], E1CUuidString[31], E1CUuidString[32], E1CUuidString[33], E1CUuidString[34], E1CUuidString[35]};
  *((char *)E1CUuid + 36) = '\0';

  std::string E2CUuidString = generateUUID(BLE.address().c_str(), "0006").c_str();
  const char* E2CUuid = new const char[36]{E2CUuidString[0], E2CUuidString[1], E2CUuidString[2], E2CUuidString[3], E2CUuidString[4], E2CUuidString[5], E2CUuidString[6], E2CUuidString[7], 
    '-', E2CUuidString[9], E2CUuidString[10], E2CUuidString[11], E2CUuidString[12], 
    '-', E2CUuidString[14], E2CUuidString[15], E2CUuidString[16], E2CUuidString[17],
    '-', E2CUuidString[19], E2CUuidString[20], E2CUuidString[21], E2CUuidString[22], 
    '-', E2CUuidString[24], E2CUuidString[25], E2CUuidString[26], E2CUuidString[27], E2CUuidString[28], E2CUuidString[29], E2CUuidString[30], E2CUuidString[31], E2CUuidString[32], E2CUuidString[33], E2CUuidString[34], E2CUuidString[35]};
  *((char *)E2CUuid + 36) = '\0';

  std::string A1CUuidString = generateUUID(BLE.address().c_str(), "0007").c_str(); 
  const char* A1CUuid = new const char[36]{A1CUuidString[0], A1CUuidString[1], A1CUuidString[2], A1CUuidString[3], A1CUuidString[4], A1CUuidString[5], A1CUuidString[6], A1CUuidString[7], 
    '-', A1CUuidString[9], A1CUuidString[10], A1CUuidString[11], A1CUuidString[12], 
    '-', A1CUuidString[14], A1CUuidString[15], A1CUuidString[16], A1CUuidString[17],
    '-', A1CUuidString[19], A1CUuidString[20], A1CUuidString[21], A1CUuidString[22], 
    '-', A1CUuidString[24], A1CUuidString[25], A1CUuidString[26], A1CUuidString[27], A1CUuidString[28], A1CUuidString[29], A1CUuidString[30], A1CUuidString[31], A1CUuidString[32], A1CUuidString[33], A1CUuidString[34], A1CUuidString[35]};
  *((char *)A1CUuid + 36) = '\0';

  std::string A2CUuidString = generateUUID(BLE.address().c_str(), "0008").c_str();  
  const char* A2CUuid = new const char[36]{A2CUuidString[0], A2CUuidString[1], A2CUuidString[2], A2CUuidString[3], A2CUuidString[4], A2CUuidString[5], A2CUuidString[6], A2CUuidString[7], 
    '-', A2CUuidString[9], A2CUuidString[10], A2CUuidString[11], A2CUuidString[12], 
    '-', A2CUuidString[14], A2CUuidString[15], A2CUuidString[16], A2CUuidString[17],
    '-', A2CUuidString[19], A2CUuidString[20], A2CUuidString[21], A2CUuidString[22], 
    '-', A2CUuidString[24], A2CUuidString[25], A2CUuidString[26], A2CUuidString[27], A2CUuidString[28], A2CUuidString[29], A2CUuidString[30], A2CUuidString[31], A2CUuidString[32], A2CUuidString[33], A2CUuidString[34], A2CUuidString[35]};
  *((char *)A2CUuid + 36) = '\0';

  std::string A3CUuidString = generateUUID(BLE.address().c_str(), "0009").c_str();
  const char* A3CUuid = new const char[36]{A3CUuidString[0], A3CUuidString[1], A3CUuidString[2], A3CUuidString[3], A3CUuidString[4], A3CUuidString[5], A3CUuidString[6], A3CUuidString[7], 
    '-', A3CUuidString[9], A3CUuidString[10], A3CUuidString[11], A3CUuidString[12], 
    '-', A3CUuidString[14], A3CUuidString[15], A3CUuidString[16], A3CUuidString[17],
    '-', A3CUuidString[19], A3CUuidString[20], A3CUuidString[21], A3CUuidString[22], 
    '-', A3CUuidString[24], A3CUuidString[25], A3CUuidString[26], A3CUuidString[27], A3CUuidString[28], A3CUuidString[29], A3CUuidString[30], A3CUuidString[31], A3CUuidString[32], A3CUuidString[33], A3CUuidString[34], A3CUuidString[35]};
  *((char *)A3CUuid + 36) = '\0';

  std::string A4CUuidString = generateUUID(BLE.address().c_str(), "0010").c_str(); 
  const char* A4CUuid = new const char[36]{A4CUuidString[0], A4CUuidString[1], A4CUuidString[2], A4CUuidString[3], A4CUuidString[4], A4CUuidString[5], A4CUuidString[6], A4CUuidString[7], 
    '-', A4CUuidString[9], A4CUuidString[10], A4CUuidString[11], A4CUuidString[12], 
    '-', A4CUuidString[14], A4CUuidString[15], A4CUuidString[16], A4CUuidString[17],
    '-', A4CUuidString[19], A4CUuidString[20], A4CUuidString[21], A4CUuidString[22], 
    '-', A4CUuidString[24], A4CUuidString[25], A4CUuidString[26], A4CUuidString[27], A4CUuidString[28], A4CUuidString[29], A4CUuidString[30], A4CUuidString[31], A4CUuidString[32], A4CUuidString[33], A4CUuidString[34], A4CUuidString[35]};
  *((char *)A4CUuid + 36) = '\0';

  std::string batteryVoltageCUuidString = generateUUID(BLE.address().c_str(), "0011").c_str();  
  const char* batteryVoltageCUuid = new const char[36]{batteryVoltageCUuidString[0], batteryVoltageCUuidString[1], batteryVoltageCUuidString[2], batteryVoltageCUuidString[3], batteryVoltageCUuidString[4], batteryVoltageCUuidString[5], batteryVoltageCUuidString[6], batteryVoltageCUuidString[7], 
    '-', batteryVoltageCUuidString[9], batteryVoltageCUuidString[10], batteryVoltageCUuidString[11], batteryVoltageCUuidString[12], 
    '-', batteryVoltageCUuidString[14], batteryVoltageCUuidString[15], batteryVoltageCUuidString[16], batteryVoltageCUuidString[17],
    '-', batteryVoltageCUuidString[19], batteryVoltageCUuidString[20], batteryVoltageCUuidString[21], batteryVoltageCUuidString[22], 
    '-', batteryVoltageCUuidString[24], batteryVoltageCUuidString[25], batteryVoltageCUuidString[26], batteryVoltageCUuidString[27], batteryVoltageCUuidString[28], batteryVoltageCUuidString[29], batteryVoltageCUuidString[30], batteryVoltageCUuidString[31], batteryVoltageCUuidString[32], batteryVoltageCUuidString[33], batteryVoltageCUuidString[34], batteryVoltageCUuidString[35]};
  *((char *)batteryVoltageCUuid + 36) = '\0';

  std::string commandStatusCUuidString = generateUUID(BLE.address().c_str(), "0012").c_str();  
  const char* commandStatusCUuid = new const char[36]{commandStatusCUuidString[0], commandStatusCUuidString[1], commandStatusCUuidString[2], commandStatusCUuidString[3], commandStatusCUuidString[4], commandStatusCUuidString[5], commandStatusCUuidString[6], commandStatusCUuidString[7], 
    '-', commandStatusCUuidString[9], commandStatusCUuidString[10], commandStatusCUuidString[11], commandStatusCUuidString[12], 
    '-', commandStatusCUuidString[14], commandStatusCUuidString[15], commandStatusCUuidString[16], commandStatusCUuidString[17],
    '-', commandStatusCUuidString[19], commandStatusCUuidString[20], commandStatusCUuidString[21], commandStatusCUuidString[22], 
    '-', commandStatusCUuidString[24], commandStatusCUuidString[25], commandStatusCUuidString[26], commandStatusCUuidString[27], commandStatusCUuidString[28], commandStatusCUuidString[29], commandStatusCUuidString[30], commandStatusCUuidString[31], commandStatusCUuidString[32], commandStatusCUuidString[33], commandStatusCUuidString[34], commandStatusCUuidString[35]};
  *((char *)commandStatusCUuid + 36) = '\0';

  std::string commandIssueCUuidString = generateUUID(BLE.address().c_str(), "0013").c_str(); 
  const char* commandIssueCUuid = new const char[36]{commandIssueCUuidString[0], commandIssueCUuidString[1], commandIssueCUuidString[2], commandIssueCUuidString[3], commandIssueCUuidString[4], commandIssueCUuidString[5], commandIssueCUuidString[6], commandIssueCUuidString[7], 
    '-', commandIssueCUuidString[9], commandIssueCUuidString[10], commandIssueCUuidString[11], commandIssueCUuidString[12], 
    '-', commandIssueCUuidString[14], commandIssueCUuidString[15], commandIssueCUuidString[16], commandIssueCUuidString[17],
    '-', commandIssueCUuidString[19], commandIssueCUuidString[20], commandIssueCUuidString[21], commandIssueCUuidString[22], 
    '-', commandIssueCUuidString[24], commandIssueCUuidString[25], commandIssueCUuidString[26], commandIssueCUuidString[27], commandIssueCUuidString[28], commandIssueCUuidString[29], commandIssueCUuidString[30], commandIssueCUuidString[31], commandIssueCUuidString[32], commandIssueCUuidString[33], commandIssueCUuidString[34], commandIssueCUuidString[35]};
  *((char *)commandIssueCUuid + 36) = '\0';

  //Connection Services

  //this is rediculous but i spent like 4 hours trying to figure out a better way and I got nothin
  //string.h steals all the memory :( - no strcpy
  static const char* test = new const char[36] {deviceServiceUUID[0], deviceServiceUUID[1], deviceServiceUUID[2], deviceServiceUUID[3], deviceServiceUUID[4], deviceServiceUUID[5], deviceServiceUUID[6], deviceServiceUUID[7], 
    '-', deviceServiceUUID[9], deviceServiceUUID[10], deviceServiceUUID[11], deviceServiceUUID[12], 
    '-', deviceServiceUUID[14], deviceServiceUUID[15], deviceServiceUUID[16], deviceServiceUUID[17],
    '-', deviceServiceUUID[19], deviceServiceUUID[20], deviceServiceUUID[21], deviceServiceUUID[22], 
    '-', deviceServiceUUID[24], deviceServiceUUID[25], deviceServiceUUID[26], deviceServiceUUID[27], deviceServiceUUID[28], deviceServiceUUID[29], deviceServiceUUID[30], deviceServiceUUID[31], deviceServiceUUID[32], deviceServiceUUID[33], deviceServiceUUID[34], deviceServiceUUID[35]};

  BLEService motorCarrierService = BLEService(test); 

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
  BLEByteCharacteristic commandIssueC(commandIssueCUuid, BLERead | BLEWrite);
  BLEByteCharacteristic commandStatusC(commandStatusCUuid, BLERead | BLEWrite);
  BLEByteCharacteristic batteryVoltageC(batteryVoltageCUuid, BLERead | BLEWrite);

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

  //stop motors by default
  M1.setDuty(0);
  M2.setDuty(0);

  //advertise the bot on the bluetooth network
  BLE.advertise();

  Serial.println("I am a bot!");

}

void loop(){
    Serial.print("Searching for central: ");
    Serial.println(BLE.address());

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

      case 104:
        //full u turn
        if(true){
          FollowLineOnMarker commandToRun(&commandStatusC, &commandIssueC, &MC);
          commandToRun.run();
          TravelStraight commandToRun2(&commandStatusC, &commandIssueC, &MC, 390);
          commandToRun2.run();
          TurnLeft commandToRun3(&commandStatusC, &commandIssueC, &MC);
          commandToRun3.run();
          TravelStraight commandToRun4(&commandStatusC, &commandIssueC, &MC, 265);
          commandToRun4.run();
          TurnLeft commandToRun5(&commandStatusC, &commandIssueC, &MC);
          commandToRun5.run();
        }
        break;

      default:
        break;
    }
  }
}


std::string generateUUID(std::string MACAddress, std::string characteristicNumber){
  
  //generatee the UUIDs for the bot's characteristics
  std::string MAP[12];

  //break down mac address
  char seperator = ':';

  for(int i = 0,j = 0; i < MACAddress.size(); i++){
      if(MACAddress[i] != seperator){
          MAP[j] = MACAddress[i];
          j++;
      }
  }

  //Scheme
  //  MACAddress 0-7 - MACAddress 8-11 - MACAddress 12-15 - characteristicNumber(as four digit number) - 000000000000

  std::string UUID = MAP[0] + MAP[1] + MAP[2] + MAP[3] + MAP[4] + MAP[5] + MAP[6] + MAP[7]
      + "-" + MAP[8] + MAP[9] + MAP[10] + MAP[11]
      + "-0000"
      + "-" + characteristicNumber[0] + characteristicNumber[1] + characteristicNumber[2] + characteristicNumber[3]
      + "-000000000000";
  
  return UUID;

}