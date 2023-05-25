#ifndef command_h
#define command_h

#include <Arduino.h>
#include <ArduinoBLE.h>


class Command{
    public:
        Command(BLECharacteristic StatusC);
        void getValue();

        BLECharacteristic m_statusC;


};


#endif