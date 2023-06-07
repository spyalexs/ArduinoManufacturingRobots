#ifndef command_h
#define command_h

#include <Arduino.h>
#include <ArduinoBLE.h>

#include "RobotContainer.h"

class Command{
    public:
        Command(BLECharacteristic* StatusC, BLECharacteristic* IssueC, RobotContainer* MC, String name);

        int getStatus();
        void updateStatus(int Status);

        bool checkForAbort();
        bool checkForConfirmation();

        virtual void startup();
        virtual void cycle();
        virtual bool ifEnd();
        virtual void cleanup();

        void run();
        void superCycle();

        RobotContainer* mp_MC;

        BLECharacteristic* m_statusC;
        BLECharacteristic* m_issueC;

        String m_name;


};


#endif