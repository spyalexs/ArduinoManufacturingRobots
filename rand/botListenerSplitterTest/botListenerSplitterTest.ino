#include <Arduino.h>

#include <WiFiNINA.h>
#include <WiFiUdp.h>

#define BUFFER_LENGTH 1000

//wifi things
String wifiSSID = "TroublingOmen";
String wifiPWD = "DarkOmen";
const char* hostName = "MiniBot";

int wifiStatus = 0;

String macString = "";
IPAddress localIPAddress;
byte macAddress[6];
int localUDPPort = 5005;

IPAddress serverIPAddress;

WiFiUDP Udp;

byte packetBuffer[BUFFER_LENGTH];


void setup(){
  //ensure board has wifi modules
    if(WiFi.status() == WL_NO_MODULE){
        Serial.print("Failed to start WIFI");
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
    macString = "";
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
}

void loop(){

    if(Udp.parsePacket()){
        long time1 = micros();

        Udp.read(packetBuffer, BUFFER_LENGTH);
        Serial.println(micros() - time1);
        Serial.println(String((char* )packetBuffer));
    }

}