#ifndef communicator_h
#define communicator_h

#include <Arduino.h>

#include "Display.h"

#include <WiFiNINA.h>
#include <WiFiUdp.h>

#include <queue>


class Communicator{
    public:
        Communicator();
        bool connectToNetwork();
        bool connectToCentral();

        std::queue<String> checkForPackets();

        void writeMessageToCentral(String message);
        void writeMessageToCentral(String characteristic, String value);
        
        void cycle();

        void setDisplayPointer(Display* display);

        std::queue<String>* getMessageOutQueuePointer();

        String m_macString;
    private:
        WiFiSSLClient m_wifiClient;
        int m_wifiStatus = WL_IDLE_STATUS;

        String m_wifiSSID = "AMR";
        String m_wifiPWD = "Mot de passe";
        const char* m_hostName = "MiniBot";

        byte m_macAddress[6];
        IPAddress m_localIPAddress;
        IPAddress m_serverIPAddress;

        WiFiUDP m_Udp;
        uint16_t m_connectionUDPPort = 5006;
        uint16_t m_localUDPPort = 5005;
        uint16_t m_assignedUDPPort = 0;

        const int m_bufferSize = 1000;
        byte* m_packetBuffer;

        bool m_ready = false; //if the wifi module is ready to commincate
        bool m_connected = false; //if the commincator is connected and has a port #
        double m_connectionTimeout = 1000; //timeout for connection protocol

        Display* mp_display = nullptr; // a pointer to the display

        std::queue<String> m_pendingMessages;


        double getTime();
};

#endif