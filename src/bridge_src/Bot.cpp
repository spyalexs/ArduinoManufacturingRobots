#include "Bot.h"

Bot::Bot(BLEDevice* bot, int number){
    //this should run after connected() and discoverAttributes()
    m_isConnected = true;

    //generate basic information
    this->mp_bot = bot;
    this->m_number = number;
    this->m_name = "bot" + String(number);

    //harvest data from bot
    std::string macAdress = mp_bot->address().c_str();
    Serial.print("* Device name: ");
    Serial.println(mp_bot->localName());
    Serial.print("* Advertised service UUID: ");
    Serial.println(mp_bot->advertisedServiceUuid());

    //assign UUids from Characteristics and Mac Address
    const char* bot1_LEDCUuid;
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
}

void Bot::cycle(){
    //perform nessecary processes and communication

}

void Bot::sustainConnection(){
    //three main steps
    //1. check for connection / update characteristics
    //2. if connection lost attempt to reaqquire
    //3. if failed attempt to reaqquire, try again in a while(loop through for a time)

}

std::string Bot::generateUUID(std::string MACAddress, std::string characteristicNumber){
    std::string MAP[16];

    //break down mac address
    char seperator = ':';

    for(int i = 0,j = 0; i < MACAddress.size(); i++){
        if(MACAddress[i] != seperator){
            MAP[j] = MACAddress[i];
            j++;
        }
    }


    //Scheme
    //  MACAddress 0-7 - MACAddress 8-11 - MACAddress 12-15 - characteristicNumber(as four digit number) - 000000000000

    std::string UUID = MAP[0] + MAP[1] + MAP[2] + MAP[3] + MAP[4] + MAP[5] + MAP[6] + MAP[7]
        + "-" + MAP[8] + MAP[9] + MAP[10] + MAP[11]
        + "-" + MAP[12] + MAP[13] + MAP[14] + MAP[15]
        + "-" + characteristicNumber[0] + characteristicNumber[1] + characteristicNumber[2] + characteristicNumber[3]
        + "-00000000";
    
    return UUID;

}
