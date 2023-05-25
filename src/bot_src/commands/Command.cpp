#include "Command.h"

Command::Command(BLECharacteristic Status){
    m_statusC = Status;
}

void Command::getValue(){
    byte value = 0;
    m_statusC.readValue(value);
    Serial.println(int(value));
    
}