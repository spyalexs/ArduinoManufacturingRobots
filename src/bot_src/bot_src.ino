//libs
#include <ArduinoMotorCarrier.h>
#include <string>
#include <queue>

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


int lastUpdate = 0; //last time the update was sent to central
int updateFrequency = 1; //Hz

Command runningCommand;
 
void setup(){
  Serial.begin(9600);

  //start up motor carrier
  if(!controller.begin()){
    Serial.println("Failed to start controller!");
  }


  Serial.println("I am a bot!");
  Serial.println("I was assigned: " + String(assignedUDPPort));
}

void loop(){
  //nothing can be blocking!

  //update central with general information
  update();

  //listen to central for commands
  listen();
}


// void commandLoop(){
// //this runs while the bot is connected to the bridge

//   if(CC.readIssue() != 0) {
//     //if the robot is in command mode - normal operation 

//     //get which command and then write over it to over doing the command multiple times unnessecarily
//     int commandVal = CC.readIssue();
//     CC.writeIssue(0);

//     //run commands
//     //0 - no command, shouldn't be in this switch anyways
//     //1 - testing
//     //2 to 99 - base actions
//     //101 to 199 - compound actions
//     //255 - abort!
//     switch (commandVal){
//       case 1:
//         if(true){
//           TestCommand commandToRun(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
//           commandToRun.run();
//         }
//         break;

//       case 2:
//         if(true){
//           FollowLineUntilMarker commandToRun(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
//           commandToRun.run();
//         }
//         break;

//       case 3:
//         if(true){
//           FollowLineOnMarker commandToRun(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
//           commandToRun.run();
//         }
//         break;

//       case 4:
//         if(true){
//           TravelStraight commandToRun(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC, 210);
//           commandToRun.run();
//         }
//         break;

//       case 5:
//         if(true){
//           TurnRight commandToRun(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
//           commandToRun.run();
//         }
//         break;

//       case 6:
//         if(true){
//           TurnLeft commandToRun(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
//           commandToRun.run();
//         }
//         break;

//       case 101:
//         //full right turn through intersection
//         if(true){
//           FollowLineOnMarker commandToRun(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
//           commandToRun.run();
//           TravelStraight commandToRun2(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC, 210);
//           commandToRun2.run();
//           TurnRight commandToRun3(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
//           commandToRun3.run();
//         }
//         break;

//       case 102:
//         //full straight through intersection
//         if(true){
//           FollowLineOnMarker commandToRun(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
//           commandToRun.run();
//           TravelStraight commandToRun2(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC, 200);
//           commandToRun2.run();
//           FollowLineUntilMarker commandToRun3(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
//           commandToRun3.run();
//           TravelStraight commandToRun4(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC, 100);
//           commandToRun4.run();

//         }
//         break;

//       case 103:
//         //full left through intersection
//         if(true){
//           FollowLineOnMarker commandToRun(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
//           commandToRun.run();
//           TravelStraight commandToRun2(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC, 200);
//           commandToRun2.run();
//           FollowLineUntilMarker commandToRun3(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
//           commandToRun3.run();
//           TravelStraight commandToRun4(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC, 310);
//           commandToRun4.run();
//           TurnLeft commandToRun5(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
//           commandToRun5.run();
//           TravelStraight commandToRun6(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC, 40);
//           commandToRun6.run();
//         }
//         break;

//       case 104:
//         //full u turn
//         if(true){
//           FollowLineOnMarker commandToRun(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
//           commandToRun.run();
//           TravelStraight commandToRun2(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC, 390);
//           commandToRun2.run();
//           TurnLeft commandToRun3(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
//           commandToRun3.run();
//           TravelStraight commandToRun4(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC, 265);
//           commandToRun4.run();
//           TurnLeft commandToRun5(CC.GetStatusCPointer(), CC.GetIssueCPointer(), &MC);
//           commandToRun5.run();
//         }
//         break;

//       default:
//         break;
//     }
//   }
// }

void update(){
  double time = MC.getTime();

  // if it is time to update central
  if(lastUpdate + (1 / updateFrequency) < time){
    lastUpdate = time;

    //update must be less than 50 bytes
    String update = "bat$" + String(MC.getBatteryVoltage()) + "$$$";

    update.getBytes(packetBuffer, bufferSize);
    Udp.beginPacket(serverIPAddress, assignedUDPPort);
    Udp.write(packetBuffer, bufferSize);
    Udp.endPacket();
  }
}

int listen(){
  //listen for updates from central
  //return the command number if there is ine

  if(Udp.parsePacket()){
    //read in packet
    Udp.read(packetBuffer, bufferSize);

    //copy packet to string to be processed
    String packet = (char*)packetBuffer;

    
  }

  return 0;
}

