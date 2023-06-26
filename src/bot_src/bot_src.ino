//libs
#include <ArduinoMotorCarrier.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <string>

#include "RobotContainer.h"

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

WiFiSSLClient wifiClient;
int wifiStatus = WL_IDLE_STATUS;

String wifiSSID = "TroublingOmen";
String wifiPWD = "DarkOmen";
const char* hostName = "MiniBot";

byte macAddress[6];
IPAddress localIPAddress;
IPAddress serverIPAddress;

WiFiUDP Udp;
uint16_t localUDPPort = 5005;
uint16_t assignedUDPPort = 0;

const int bufferSize = 50;
byte packetBuffer[bufferSize];

int lastUpdate = 0; //last time the update was sent to central
int updateFrequency = 1; //Hz
 
void setup(){
  Serial.begin(9600);

  //start up motor carrier
  if(!controller.begin()){
    Serial.println("Failed to start controller!");
  }

  //ensure board has wifi module
  if(WiFi.status() == WL_NO_MODULE){
    Serial.print("Failed to start WIFI");
    while(true);
  }

  WiFi.setHostname(hostName);

  //attempt to connect to access point
  while(wifiStatus != WL_CONNECTED){
    Serial.print("Attempting To connect to: ");
    Serial.println(wifiSSID);

    wifiStatus = WiFi.begin(wifiSSID.c_str(), wifiPWD.c_str());

    delay(1000);
    Serial.print("Connection Status: ");
    Serial.println(wifiStatus);  
  }

  //Print Mac and Ip Address
  WiFi.macAddress(macAddress);

  //get mac string
  String macString = "";
  for(int i = 5; i >= 0; i--){
    macString = macString + String(macAddress[i], HEX);
    if(i > 0){
      macString = macString + ":";
    }
  }
  Serial.println(macString);

  //ip address  
  localIPAddress = WiFi.localIP();
  Serial.print("My IP is: ");
  Serial.println(localIPAddress);
  
  //get server ip address
  serverIPAddress = localIPAddress;
  serverIPAddress[3] = 1;
  Serial.print("Server IP is: ");
  Serial.println(serverIPAddress);

  //start UDP server
  if(!Udp.begin(localUDPPort)){
    Serial.println("Failed to begin UDP");
  }

  bool connecting = true; //while waiting from connection from central

  //connect to central controller and get a COM slot
  while(connecting){
    //listen for port assignment

    if(Udp.parsePacket()){
      Serial.println("Got Something");

      Udp.read(packetBuffer, bufferSize);

      //find port number
      for(int i = 0; i < bufferSize; i++){
        //port number is right after colcon
        if((char)packetBuffer[i] == ':'){
          //get number from next four bytes
          assignedUDPPort = 1000 * ((int)packetBuffer[i+1] - 48) + 100 * ((int)packetBuffer[i+2] - 48) + 10 * ((int)packetBuffer[i+3] - 48) + ((int)packetBuffer[i+4] - 48);
        }
      }

      //move out of connection phase
      connecting = false;
      break;
    }

    //write message to get central's attentions
    String message = "I'm a bot! MAC:" + macString + "$$$";
    message.getBytes(packetBuffer, bufferSize);
    Udp.beginPacket(serverIPAddress, localUDPPort);
    Udp.write(packetBuffer, bufferSize);
    Udp.endPacket();
  }

  Serial.println("I am a bot!");
  Serial.println("I was assigned: " + String(assignedUDPPort));
}

void loop(){
  //nothing can be blocking!

  //update central with general information
  update();

  //listen to central for commands
  
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

