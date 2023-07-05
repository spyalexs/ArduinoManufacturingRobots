#include "Communicator.h"

Communicator::Communicator(){
    m_packetBuffer = new byte[m_bufferSize];
    m_ready = false;
}

bool Communicator::connectToNetwork(){
    //ensure board has wifi module
    if(WiFi.status() == WL_NO_MODULE){
        Serial.print("Failed to start WIFI");
        return false;
    }

    WiFi.setHostname(this->m_hostName);

    double timeoutTime = this->getTime() + m_connectionTimeout;

    //attempt to connect to access point
    while(this->m_wifiStatus != WL_CONNECTED){
        Serial.print("Attempting To connect to: ");
        Serial.println(this->m_wifiSSID);

        this->m_wifiStatus = WiFi.begin(this->m_wifiSSID.c_str(), this->m_wifiPWD.c_str());

        delay(1000);
        Serial.print("Connection Status: ");
        Serial.println(this->m_wifiStatus);  

        //break if timeout
        if(timeoutTime < this->getTime()){
            Serial.println("Connection Timeout. Could not connect to network!");
            return false;
        }
    }

    //Print Mac and Ip Address
    WiFi.macAddress(this->m_macAddress);

    //get mac string
    this->m_macString = "";
    for(int i = 5; i >= 0; i--){
        this->m_macString = this->m_macString + String(this->m_macAddress[i], HEX);
        if(i > 0){
            this->m_macString = this->m_macString + ":";
        }
    }
    Serial.println(this->m_macString);

    //ip address  
    this->m_localIPAddress = WiFi.localIP();
    Serial.print("My IP is: ");
    Serial.println(this->m_localIPAddress);
    
    //get server ip address
    this->m_serverIPAddress = this->m_localIPAddress;
    this->m_serverIPAddress[3] = 1;
    Serial.print("Server IP is: ");
    Serial.println(this->m_serverIPAddress);

    //start UDP server
    if(!this->m_Udp.begin(this->m_localUDPPort)){
        Serial.println("Failed to begin UDP");

        return false;
    }

    this->m_ready = true;
}

bool Communicator::connectToCentral(){
    //make sure the communicator initailized properly
    if(!this->m_ready){
        Serial.println("Cannot connect to central... network connection not ready!");
        //TODO: make a fallback
        return false;
    }

    double timeoutTime = millis() / 1000000 + m_connectionTimeout;

    //connect to the central script and get a port
    while(!this->m_connected){
        //listen for port assignment

        if(this->m_Udp.parsePacket()){

            //read in the parsed packet
            this->m_Udp.read(this->m_packetBuffer, this->m_bufferSize);

            //find port number
            for(int i = 0; i < this->m_bufferSize; i++){
                //port number is right after colcon
                if((char)m_packetBuffer[i] == ':'){
                    //get number from next four bytes
                    this->m_assignedUDPPort = 1000 * ((int)this->m_packetBuffer[i+1] - 48) + 100 * ((int)m_packetBuffer[i+2] - 48) + 10 * ((int)m_packetBuffer[i+3] - 48) + ((int)m_packetBuffer[i+4] - 48);
                }
            }

                //move out of connection phase
                this->m_connected = true;
                break;
            }

        //write message to get central's attentions
        String message = "Im a bot! MAC:" + this->m_macString + "$$$";
        message.getBytes(this->m_packetBuffer, this->m_bufferSize);
        this->m_Udp.beginPacket(this->m_serverIPAddress, this->m_connectionUDPPort);
        this->m_Udp.write(this->m_packetBuffer, this->m_bufferSize);
        this->m_Udp.endPacket();

        //check for timeout
        if(millis() / 1000000 > timeoutTime){
            Serial.println("Connection Timeout. Could not connect to central program!");
            return false;
        }
    }

    Serial.println("I was assigned: " + String(m_assignedUDPPort));

    return true;
}

double Communicator::getTime(){
    //returnt the system time
    return millis() / 1000000;
}

void Communicator::writeMessageToCentral(String message){
    //send a packet to the central server

    String messageString = message + "$$$"; // add ending to message

    messageString.getBytes(this->m_packetBuffer, this->m_bufferSize);
    this->m_Udp.beginPacket(this->m_serverIPAddress, this->m_assignedUDPPort);
    this->m_Udp.write(this->m_packetBuffer, this->m_bufferSize);
    this->m_Udp.endPacket();
}

void Communicator::writeMessageToCentral(String characteristic, String value){
    //send a packet to the central server

    String messageString = characteristic + "$" + value + "$$$"; // add ending to message

    messageString.getBytes(this->m_packetBuffer, this->m_bufferSize);
    this->m_Udp.beginPacket(this->m_serverIPAddress, this->m_assignedUDPPort);
    this->m_Udp.write(this->m_packetBuffer, this->m_bufferSize);
    this->m_Udp.endPacket();
}

void Communicator::cycle(){

}

std::queue<String> Communicator::checkForPackets(){
    //return a packet if there is one
    //if not return an empty string ""

    std::queue<String> packets;

    //go through and collect all packets that came in 
    while(this->m_Udp.parsePacket()){

        this->m_Udp.readBytes(m_packetBuffer, m_bufferSize);

        //this only works when there is a "\0" character at the end of the string
        String packetMessage = String((char*)m_packetBuffer);
        packets.push(packetMessage);

    }
    
    return packets;
}