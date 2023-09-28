//libs
#include <ArduinoMotorCarrier.h>
#include <string>
#include <queue>

#include "RobotContainer.h"
#include "Communicator.h"
#include "Sequencer.h"
#include "CycleOverseer.h"

//commands
#include "TurnRight.h"
#include "TurnLeft.h"
#include "FollowLineUntilMarker.h"
#include "TravelStraight.h"
#include "FollowLineOnMarker.h"
#include "TestCommand.h"


bool PUBLISHDATA = true;

//the robot container that is a wrapper around periphral functions
RobotContainer MC = RobotContainer(&M1, &M2, &encoder1, &encoder2, IN1, A6, A2);
Communicator CC = Communicator();
Sequencer SC = Sequencer();
CycleOverseer CO = CycleOverseer(50);

//GPIOboard
Adafruit_MCP23X17 gpio;

int lastUpdate = 0; //last time the update was sent to central
int updateFrequency = 1; //Hz

Command* runningCommand;

bool inMenu = true; // wether or not the bot is in menu mode
 
void setup(){
  Serial.begin(9600);

  SC.setCommunicatorPointer(&CC);
  CC.setDisplayPointer(&MC.m_display);
  MC.m_display.setMessagesOutQueue(CC.getMessageOutQueuePointer());

  //start up motor carrier
  if(!controller.begin()){
    Serial.println("Failed to start controller!");
  }

  //startup gpio board
  if(!gpio.begin_I2C()){
    Serial.println("Failed to start the gpio board!");
  }

  //set the robot container gpio pointer
  MC.setGPIOPointer(&gpio);

  //set up display basics
  MC.m_display.setBackground(ILI9341_BLACK);
  MC.m_display.drawBasicUI();

  MC.m_display.drawOpeningMenu();
  //MC.BypassEncoder();

  //get coms set up with central
  CC.connectToNetwork();
  CC.connectToCentral();

  Serial.println("I am a bot!");

  delay(1000);
}

void loop(){
  //nothing can be blocking

  //run running command
  if(runningCommand != nullptr){
    //if the running command is real

    //if the running command is finished
    if(runningCommand->isCompleted()){
      //set the running command to nothing - next cycle it will pick up the next command
      runningCommand = nullptr;
      SC.removeCurrentCommand();
    }else{
      //run the running command
      runningCommand->superCycle();
    }
  } else {
    //if there is a command that needs run
    if(!SC.isEmpty()){
      runningCommand = SC.getCurrentCommand();

      //begin the running command
      runningCommand->run();
    }
  }
  CO.clock();

  //update central with general information
  update();
  CO.clock();

  //listen to central for commands
  listen();
  CO.clock();

  //cycle communicator
  CC.cycle();
  CO.clock();

  //cycle robot container
  MC.cycle();
  CO.endCycle();
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
        SC.loadInCommand(TestCommand(&MC, &CC));
      }
      break;

    case 2:
      if(true){
        SC.loadInCommand(FollowLineUntilMarker(&MC, &CC));
      }
      break;

    case 3:
      if(true){
        SC.loadInCommand(FollowLineOnMarker(&MC, &CC));
      }
      break;

    case 4:
      if(true){
        SC.loadInCommand(TravelStraight(&MC, &CC, 1000));
      }
      break;

    case 5:
      if(true){
        SC.loadInCommand(TurnRight(&MC, &CC));
      }
      break;

    case 6:
      if(true){
        SC.loadInCommand(TurnLeft(&MC, &CC));
      }
      break;

    case 101:
      //full right turn through intersection
      if(true){
        SC.loadInCommand(FollowLineOnMarker(&MC, &CC));
        SC.loadInCommand(TravelStraight(&MC, &CC, 210));
        SC.loadInCommand(TurnRight(&MC, &CC));
      }
      break;

    case 102:
      //full straight through intersection
      if(true){
        SC.loadInCommand(FollowLineOnMarker(&MC, &CC));
        SC.loadInCommand(TravelStraight(&MC, &CC, 200));
        SC.loadInCommand(FollowLineUntilMarker(&MC, &CC));
        SC.loadInCommand(TravelStraight(&MC, &CC, 100));
      }
      break;

    case 103:
      //full left through intersection
      if(true){
        SC.loadInCommand(FollowLineOnMarker(&MC, &CC));
        SC.loadInCommand(TravelStraight(&MC, &CC, 200));
        SC.loadInCommand(FollowLineUntilMarker(&MC, &CC));
        SC.loadInCommand(TravelStraight(&MC, &CC, 310));
        SC.loadInCommand(TurnLeft(&MC, &CC));
        SC.loadInCommand(TravelStraight(&MC, &CC, 40));
      }
      break;

    case 104:
      //full u turn
      if(true){
        SC.loadInCommand(FollowLineOnMarker(&MC, &CC));
        SC.loadInCommand(TravelStraight(&MC, &CC, 390));
        SC.loadInCommand(TurnLeft(&MC, &CC));
        SC.loadInCommand(TravelStraight(&MC, &CC, 265));
        SC.loadInCommand(TurnLeft(&MC, &CC));
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

    if(characteristic == "commandIssue"){
      //in this case, the value is in int
      int value = dataString.substring(secondSeperator + 1).toInt();

      switch (value){
        case 255:
          //abort running command and cancel all other commands in queue
          if(runningCommand != nullptr){
              runningCommand->abort();
          } else {
            Serial.println("Abort message recieved, however, there is no command to abort!");
          }
        case 254:
          //clear all commands from queue
          SC.clear();
          break;

        case 253: 
          //confirm the currently running command
          if(runningCommand != nullptr){
              runningCommand->confirmCommand();
          }
          break;

        default:
          assignCommand(value);
        break;
      }
    } else if (characteristic == "destination"){
      //value is a string containing the destination
      String destination = dataString.substring(secondSeperator + 1);

      //write the destination
      MC.m_display.updateDestination(destination);

    } else if (characteristic == "SetSlotCount"){
      int value = dataString.substring(secondSeperator + 1).toInt();

      //wipe display and adjust icon count
      MC.m_display.addWipeDisplayJob(false, 0);

      MC.m_display.setIconsCount(value);

    } else if (characteristic == "AddItem"){
      // add an item to a slot
      String itemName = dataString.substring(secondSeperator + 1);
      
      MC.m_display.addItem(itemName);
    } else if (characteristic == "RemoveItem"){
      // remove an item from a slot
      String itemName = dataString.substring(secondSeperator + 1);
      
      MC.m_display.removeItem(itemName);
    }

    //remove packet after it has been handled
    incomingPackets.pop();
  }
}

