#ifndef command_h
#define command_h

#include <Arduino.h>
#include <ArduinoBLE.h>


class Command{
    public:
        Command(BLECharacteristic* StatusC, BLECharacteristic* IssueC);

        void getStatus();
        void updateStatus(int Status);
        void checkForAbort();

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