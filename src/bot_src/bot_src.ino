//libs
#include <ArduinoBLE.h>
#include <ArduinoMotorCarrier.h>
#include <string>

#include "RobotContainer.h"
#include "Communicator.h"

//commands
#include "TurnRight.h"
#include "TurnLeft.h"
#include "FollowLineUntilMarker.h"
#include "TravelStraight.h"
#include "FollowLineOnMarker.h"
#include "TestCommand.h"

bool PUBLISHDATA = true;

//the robot container that is a wrapper around periphral functions
RobotContainer MC = RobotContainer(&M1, &M2, &encoder1, &encoder2, A3, A6, A2);

//initialize characteristics as dummies ---------------------- fill in later
std::string* serviceUUID = (std::string*)malloc(sizeof(std::string));
 
void setup(){
  delay(5000);

  //start up led
  pinMode(LED_BUILTIN, OUTPUT);

  //start up motor controller
  if(!controller.begin()){
    Serial.println("Failed to start motor controller!");
  }

  //stop motors by default
  M1.setDuty(0);
  M2.setDuty(0);

  Serial.println("I am a bot!");
}

void loop(){

  if(!BLE.begin()){
    Serial.println("Failed to start BLE!");
  }
  
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

  std::string LEDCUuidString = generateUUID(BLE.address().c_str(), "0001").c_str();
  const char* LEDCUuid = new const char[36]{LEDCUuidString[0], LEDCUuidString[1], LEDCUuidString[2], LEDCUuidString[3], LEDCUuidString[4], LEDCUuidString[5], LEDCUuidString[6], LEDCUuidString[7], 
      '-', LEDCUuidString[9], LEDCUuidString[10], LEDCUuidString[11], LEDCUuidString[12], 
      '-', LEDCUuidString[14], LEDCUuidString[15], LEDCUuidString[16], LEDCUuidString[17],
      '-', LEDCUuidString[19], LEDCUuidString[20], LEDCUuidString[21], LEDCUuidString[22], 
      '-', LEDCUuidString[24], LEDCUuidString[25], LEDCUuidString[26], LEDCUuidString[27], LEDCUuidString[28], LEDCUuidString[29], LEDCUuidString[30], LEDCUuidString[31], LEDCUuidString[32], LEDCUuidString[33], LEDCUuidString[34], LEDCUuidString[35]};
  *((char *)LEDCUuid + 36) = '\0';

  Serial.println("Creating Communicator");
  Communicator CC = Communicator(commandIssueCUuid, commandStatusCUuid, LEDCUuid);
  CC.prepareCharacteristics();
  
  while(true){
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
        //connectedLoop(bridge, CC);

        Serial.println(CC.readIssue());
        Serial.println(CC.GetIssueCPointer()->uuid());
      }

      //need to take a look at this - even when definetly disconnected, the bot never seems to register, although bridge easily reconnects
      Serial.println("Disconnected from the bridge! - Stopping until connection reaquirred.");
    }
  }
}

void connectedLoop(BLEDevice testd, Communicator CC){
//this runs while the bot is connected to the bridge

  if(CC.readIssue() != 0) {
    //if the robot is in command mode - normal operation 

    //get which command and then write over it to over doing the command multiple times unnessecarily
    int commandVal = CC.readIssue();
    CC.writeIssue(0);

    //run commands
    //0 - no command, shouldn't be in this switch anyways
    //1 - testing
    //2 to 99 - base actions
    //101 to 199 - compound actions
    //255 - abort!
    switch (commandVal){
      case 1:
        if(true){
          TestCommand commandToRun(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
          commandToRun.run();
        }
        break;

      case 2:
        if(true){
          FollowLineUntilMarker commandToRun(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
          commandToRun.run();
        }
        break;

      case 3:
        if(true){
          FollowLineOnMarker commandToRun(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
          commandToRun.run();
        }
        break;

      case 4:
        if(true){
          TravelStraight commandToRun(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC, 210);
          commandToRun.run();
        }
        break;

      case 5:
        if(true){
          TurnRight commandToRun(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
          commandToRun.run();
        }
        break;

      case 6:
        if(true){
          TurnLeft commandToRun(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
          commandToRun.run();
        }
        break;

      case 101:
        //full right turn through intersection
        if(true){
          FollowLineOnMarker commandToRun(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
          commandToRun.run();
          TravelStraight commandToRun2(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC, 210);
          commandToRun2.run();
          TurnRight commandToRun3(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
          commandToRun3.run();
        }
        break;

      case 102:
        //full straight through intersection
        if(true){
          FollowLineOnMarker commandToRun(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
          commandToRun.run();
          TravelStraight commandToRun2(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC, 200);
          commandToRun2.run();
          FollowLineUntilMarker commandToRun3(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
          commandToRun3.run();
          TravelStraight commandToRun4(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC, 100);
          commandToRun4.run();

        }
        break;

      case 103:
        //full left through intersection
        if(true){
          FollowLineOnMarker commandToRun(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
          commandToRun.run();
          TravelStraight commandToRun2(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC, 200);
          commandToRun2.run();
          FollowLineUntilMarker commandToRun3(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
          commandToRun3.run();
          TravelStraight commandToRun4(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC, 310);
          commandToRun4.run();
          TurnLeft commandToRun5(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
          commandToRun5.run();
          TravelStraight commandToRun6(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC, 40);
          commandToRun6.run();
        }
        break;

      case 104:
        //full u turn
        if(true){
          FollowLineOnMarker commandToRun(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
          commandToRun.run();
          TravelStraight commandToRun2(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC, 390);
          commandToRun2.run();
          TurnLeft commandToRun3(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
          commandToRun3.run();
          TravelStraight commandToRun4(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC, 265);
          commandToRun4.run();
          TurnLeft commandToRun5(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
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


