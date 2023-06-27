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
Communicator CC = Communicator();

int lastUpdate = 0; //last time the update was sent to central
int updateFrequency = 1; //Hz

Command* runningCommand;
std::queue<int> commandQueue;
 
void setup(){
  Serial.begin(9600);

  //start up motor carrier
  if(!controller.begin()){
    Serial.println("Failed to start controller!");
  }

  //get coms set up with central
  CC.connectToNetwork();
  CC.connectToCentral();

  Serial.println("I am a bot!");
}

void loop(){
  //nothing can be blocking!

  
  //run running command
  if(runningCommand != nullptr){
    //if the running command is real

    //if the running command is finished
    if(runningCommand->isCompleted()){
      //set the running command to nothing - next cycle it will pick up the next command
      runningCommand = nullptr;
      commandQueue.pop();
    }

    //run the running command
    runningCommand->superCycle();
  } else {
    //if there is a command that needs run
    if(!commandQueue.empty()){
      runningCommand = &commandQueue.front();

      //begin the running command
      runningCommand->run();
    }
  }

  //update central with general information
  update();

  //listen to central for commands
  listen();
}


void assignCommand(int commandNumber){
  //add a command to the queue of commands to run

  //run commands
  //0 - no command, shouldn't be in this switch anyways
  //1 - testing
  //2 to 99 - base actions
  //101 to 199 - compound actions
  //255 - abort!
  switch (commandNumber){
    case 1:
      if(true){
        commandQueue.push(TestCommand(&MC, &CC));
      }
      break;

    case 2:
      if(true){
        commandQueue.push(FollowLineUntilMarker(&MC, &CC));
      }
      break;

    case 3:
      if(true){
        commandQueue.push(FollowLineOnMarker(&MC, &CC));
      }
      break;

    case 4:
      if(true){
        commandQueue.push(TravelStraight(&MC, &CC, 210));
      }
      break;

    case 5:
      if(true){
        commandQueue.push(TurnRight(&MC, &CC));
      }
      break;

    case 6:
      if(true){
        commandQueue.push(TurnLeft(&MC, &CC));
      }
      break;

    case 101:
      //full right turn through intersection
      if(true){
        commandQueue.push(FollowLineOnMarker(&MC, &CC));
        commandQueue.push(TravelStraight(&MC, &CC, 210));
        commandQueue.push(TurnRight(&MC, &CC));
      }
      break;

    case 102:
      //full straight through intersection
      if(true){
        commandQueue.push(FollowLineOnMarker(&MC, &CC));
        commandQueue.push(TravelStraight(&MC, &CC, 200));
        commandQueue.push(FollowLineUntilMarker(&MC, &CC));
        commandQueue.push(TravelStraight(&MC, &CC, 100));
      }
      break;

    case 103:
      //full left through intersection
      if(true){
        commandQueue.push(FollowLineOnMarker(&MC, &CC));
        commandQueue.push(TravelStraight(&MC, &CC, 200));
        commandQueue.push(FollowLineUntilMarker(&MC, &CC));
        commandQueue.push(TravelStraight(&MC, &CC, 310));
        commandQueue.push(TurnLeft(&MC, &CC));
        commandQueue.push(TravelStraight(&MC, &CC, 40));
      }
      break;

    case 104:
      //full u turn
      if(true){
        commandQueue.push(FollowLineOnMarker(&MC, &CC));
        commandQueue.push(TravelStraight(&MC, &CC, 390));
        commandQueue.push(TurnLeft(&MC, &CC));
        commandQueue.push(TravelStraight(&MC, &CC, 265));
        commandQueue.push(TurnLeft(&MC, &CC));
      }
      break;

    default:
      break;
  }
}

void update(){
  //send information to central
  //something must be sent every five seconds to maintain connection

  double time = MC.getTime();

  // if it is time to update central
  if(lastUpdate + (1 / updateFrequency) < time){
    lastUpdate = time;

    //update must be less than 50 bytes
    CC.writeMessageToCentral("bat", String(MC.getBatteryVoltage()));
  }
}

void listen(){
  // listen and handle messages from the central controller

  std::queue<String> incomingPackets = CC.checkForPackets();

  //go through all the packets and handle each
  while(!incomingPackets.empty()){
    String packet = incomingPackets.front();

    //find the seperator in the packet
    int firstSeperator = packet.indexOf("$");
    String dataString = packet.substring(firstSeperator + 1);
    int secondSeperator = dataString.indexOf("$");
    String characteristic = dataString.substring(0, secondSeperator);
    int value = dataString.substring(secondSeperator + 1).toInt();

    if(characteristic == "commandIssue"){
      switch (value){
        case 255:
          //abort running command and cancel all other commands in queue
          runningCommand->abort();
        case 254:
          //clear all commands from queue
          while(!commandQueue.empty()){
            commandQueue.pop();
          }
          break;

        case 253: 
          //confirm the currently running command
          runningCommand->confirmCommand();
          break;

        default:
          addCommand(value);
        break;
      }
    }

    //remove packet after it has been handled
    incomingPackets.pop();
  }
}

