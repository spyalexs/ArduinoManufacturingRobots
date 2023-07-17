//libs
#include <ArduinoMotorCarrier.h>
#include <string>
#include <queue>

#include "StationContainer.h"
#include "Communicator.h"
#include "CycleOverseer.h"

bool PUBLISHDATA = true;

//the robot container that is a wrapper around periphral functions
StationContainer MC = StationContainer();
Communicator CC = Communicator();
CycleOverseer CO = CycleOverseer(50);

int lastUpdate = 0; //last time the update was sent to central
int updateFrequency = 1; //Hz

bool inMenu = true; // wether or not the bot is in menu mode
 
long startTime = 0;
void setup(){
  Serial.begin(9600);

  CC.setDisplayPointer(&MC.m_display);
  MC.m_display.setMessagesOutQueue(CC.getMessageOutQueuePointer());

  //start up motor carrier
  if(!controller.begin()){
    Serial.println("Failed to start controller!");
  }

  //set up display basics
  MC.m_display.setBackground(ILI9341_BLACK);
  MC.m_display.drawBasicUI();

  MC.m_display.drawBasicUI();

  //get coms set up with central
  CC.connectToNetwork();
  CC.connectToCentral();

  Serial.println("I am a station!");

  delay(1000);

  MC.m_display.updateItem("Mustang");

  startTime = millis() + 50000;
}

void loop(){
  //nothing can be blocking

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

  if(startTime < millis()){
    MC.m_display.beginTransfer("Test Bot");

    startTime = startTime + 10000000;
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

    if(characteristic == "transferProduct"){

    } else if(characteristic == "stockPart")

    //remove packet after it has been handled
    incomingPackets.pop();
  }
}

