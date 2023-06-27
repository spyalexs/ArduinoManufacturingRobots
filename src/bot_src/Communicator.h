#ifndef communicator_h
#define communicator_h

#include <Arduino.h>

#include <WiFiNINA.h>
#include <WiFiUdp.h>


class Communicator{
    public:
        Communicator();
        bool connectToNetwork();
        bool connectToCentral();

        void writeMessageToCentral(String message);
        void writeMessageToCentral(String characteristic, String value);
        void cycle();

    private:
        WiFiSSLClient m_wifiClient;
        int m_wifiStatus = WL_IDLE_STATUS;

        String m_wifiSSID = "TroublingOmen";
        String m_wifiPWD = "DarkOmen";
        const char* m_hostName = "MiniBot";

        byte m_macAddress[6];
        String m_macString;
        IPAddress m_localIPAddress;
        IPAddress m_serverIPAddress;

        WiFiUDP m_Udp;
        uint16_t m_localUDPPort = 5005;
        uint16_t m_assignedUDPPort = 0;

        const int m_bufferSize = 50;
        byte* m_packetBuffer;

        bool m_ready = false; //if the wifi module is ready to commincate
        bool m_connected = false; //if the commincator is connected and has a port #
        double m_connectionTimeout = 1000; //timeout for connection protocol

        double getTime();
        String checkForPackets();
};

#endif