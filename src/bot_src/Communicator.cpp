#include "Communicator.h"

Communicator::Communicator(){
    m_packetBuffer = new byte[m_bufferSize];

    //ensure board has wifi module
    if(WiFi.status() == WL_NO_MODULE){
        Serial.print("Failed to start WIFI");
        while(true);
    }

    WiFi.setHostname(this->m_hostName);

    //attempt to connect to access point
    while(m_wifiStatus != WL_CONNECTED){
        Serial.print("Attempting To connect to: ");
        Serial.println(m_wifiSSID);

        m_wifiStatus = WiFi.begin(m_wifiSSID.c_str(), m_wifiPWD.c_str());

        delay(1000);
        Serial.print("Connection Status: ");
        Serial.println(m_wifiStatus);  
    }

    //Print Mac and Ip Address
    WiFi.macAddress(m_macAddress);

    //get mac string
    this->m_macString = "";
    for(int i = 5; i >= 0; i--){
        macString = macString + String(m_macAddress[i], HEX);
        if(i > 0){
        macString = macString + ":";
        }
    }
    Serial.println(macString);

    //ip address  
    m_localIPAddress = WiFi.localIP();
    Serial.print("My IP is: ");
    Serial.println(m_localIPAddress);
    
    //get server ip address
    m_serverIPAddress = m_localIPAddress;
    m_serverIPAddress[3] = 1;
    Serial.print("Server IP is: ");
    Serial.println(m_serverIPAddress);

    //start UDP server
    if(!m_Udp.begin(m_localUDPPort)){
        Serial.println("Failed to begin UDP");
    }

    m_ready = true;


}

bool Communicator::connect(){
    //make sure the communicator initailized properly
    if(!this->m_ready){
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
                    this->m_assignedUDPPort = 1000 * ((int)this->m_packetBuffer[i+1] - 48) + 100 * ((int)packetBuffer[i+2] - 48) + 10 * ((int)packetBuffer[i+3] - 48) + ((int)packetBuffer[i+4] - 48);
                }
            }

                //move out of connection phase
                this->m_connected = true;
                break;
            }

        //write message to get central's attentions
        String message = "I'm a bot! MAC:" + this->m_macString + "$$$";
        message.getBytes(this->m_packetBuffer, this->m_bufferSize);
        this->m_Udp.beginPacket(this->m_serverIPAddress, this->m_localUDPPort);
        this->m_Udp.write(this->m_packetBuffer, this->m_bufferSize);
        this->m_Udp.endPacket();

        //check for timeout
        if(millis() / 1000000 > timeoutTime){
            return false;
        }
    }

    return true;
}