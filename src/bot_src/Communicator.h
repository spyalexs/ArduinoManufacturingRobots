#ifndef communicator_h
#define communicator_h

#include <arduino.h>
#include <ArduinoBLE.h>
#include <string>


class Communicator{
    public:
        Communicator(const char* issueUUID, const char* statusUUID, const char* LEDUUID);
        void prepareCharacteristics();

        void writeStatus(int status);
        void writeIssue(int issue);
        void writeLED(int statusLED);
        
        int readStatus();
        int readIssue();
        int readLED();

        BLECharacteristic* GetStatusCPointer();
        BLECharacteristic* GetIssueCPointer();
        BLECharacteristic* GetLEDCPointer();

        std::string generateUUID(std::string MACAddress, std::string characteristicNumber);

    private:
        BLECharacteristic m_statusC;
        BLECharacteristic m_issueC;
        BLECharacteristic m_LEDC;

        bool m_intialized = false;

};




#endif