#ifndef bot_h
#define bot_h

#include <ArduinoBLE.h>
#include <string>

// a class to hold connection features for a single bot
class Bot{
    private:
        int m_number;
        String m_name;
        BLEDevice* mp_bot;
        bool m_mindControlState;


        //UUIDS
        std::string m_deviceServiceUUID;
        std::string m_LEDCUuid;
        std::string m_mindControlCUuid;
        std::string m_M1CUuid;
        std::string m_M2CUuid;
        std::string m_E1CUuid;
        std::string m_E2CUuid;
        std::string m_A1CUuid;
        std::string m_A2CUuid;
        std::string m_A3CUuid;
        std::string m_A4CUuid;
        std::string m_batteryVoltageUuid;
        std::string m_statusCUuid;
        std::string m_issueCUuid;

        //Characteristics
        BLECharacteristic m_LEDC;
        BLECharacteristic m_mindControlC;
        BLECharacteristic m_M1C;
        BLECharacteristic m_M2C;
        BLECharacteristic m_E1C;
        BLECharacteristic m_E2C;
        BLECharacteristic m_A1C;
        BLECharacteristic m_A2C;
        BLECharacteristic m_A3C;
        BLECharacteristic m_A4C;
        BLECharacteristic m_batteryVoltageC;
        BLECharacteristic m_issueC;
        BLECharacteristic m_statusC;


        //connection
        double m_reattemptConnection = 2.0;
        double m_lastScan = 0; // the last time the connection scan was run
        bool m_isConnected = false;

        void sustainConnection(bool isConnected);

    public:
        Bot(BLEDevice* bot, int number);
        Bot();
        void cycle(bool isConnected);

        void publishMessageToBot(String characteristic, int value);

        std::string generateUUID(std::string MacAddress, std::string CharacteristicNumber);

        bool isConnected();
        std::string getName();
        std::string getAddress();


};


#endif