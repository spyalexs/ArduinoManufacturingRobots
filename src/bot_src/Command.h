#ifndef command_h
#define command_h

#include <Arduino.h>
#include <ArduinoBLE.h>


class Command{
    public:
        Command(BLECharacteristic* StatusC, BLECharacteristic* IssueC);

        int getStatus();
        void updateStatus(int Status);
        bool checkForAbort();

        void startup();
        void cycle();
        bool ifEnd();
        void cleanup();

        void run();
        void superCycle();

        BLECharacteristic* m_statusC;
        BLECharacteristic* m_issueC;


};


#endif