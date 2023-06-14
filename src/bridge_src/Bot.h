#ifndef bot_h
#define bot_h

#include <ArduinoBLE.h>
#include <bits/stdc++.h>

// a class to hold connection features for a single bot
class Bot{
    private:
        int m_number;
        String m_name;
        BLEDevice* mp_bot;


        //UUIDS
        std::string bot1_LEDCUuid;
        const char* bot1_mindControlCUuid;
        const char* bot1_M1CUuid;
        const char* bot1_M2CUuid;
        const char* bot1_E1CUuid;
        const char* bot1_E2CUuid;
        const char* bot1_A1CUuid;
        const char* bot1_A2CUuid;
        const char* bot1_A3CUuid;
        const char* bot1_A4CUuid;
        const char* bot1_batteryVoltageUuid;
        const char* bot1_statusCUuid;
        const char* bot1_issueCUuid;


        //Characteristics
        BLECharacteristic bot1_LEDC;
        BLECharacteristic bot1_mindControlC;
        BLECharacteristic bot1_M1C;
        BLECharacteristic bot1_M2C;
        BLECharacteristic bot1_E1C;
        BLECharacteristic bot1_E2C;
        BLECharacteristic bot1_A1C;
        BLECharacteristic bot1_A2C;
        BLECharacteristic bot1_A3C;
        BLECharacteristic bot1_A4C;
        BLECharacteristic bot1_batteryVoltageC;
        BLECharacteristic bot1_issueC;
        BLECharacteristic bot1_statusC;


        //connection
        double m_reattemptConnection = 5;
        bool m_isConnected = false;

    public:
        Bot(BLEDevice* bot, int number);
        void cycle();
        void sustainConnection();

        std::string generateUUID(std::string MacAddress, std::string CharacteristicNumber)


};


#endif