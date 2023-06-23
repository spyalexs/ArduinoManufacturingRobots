#include "Bot.h"

Bot::Bot(BLEDevice* bot, int number){
  //this should run after connected() and discoverAttributes()
  m_isConnected = true;

  //generate basic information
  this->mp_bot = bot;
  this->m_number = number;
  this->m_name = "bot" + String(number);
  this->m_mindControlState = false;

  //harvest data from bot
  std::string macAddress = mp_bot->address().c_str();

  //get the service UUID 
  m_deviceServiceUUID = this->generateUUID(macAddress, "0000");

  //assign UUids from Characteristics and Mac Address
  m_LEDCUuid = this->generateUUID(macAddress, "0001");
  m_mindControlCUuid = this->generateUUID(macAddress, "0002");
  m_M1CUuid = this->generateUUID(macAddress, "0003");
  m_M2CUuid = this->generateUUID(macAddress, "0004");
  m_E1CUuid = this->generateUUID(macAddress, "0005");
  m_E2CUuid = this->generateUUID(macAddress, "0006");
  m_A1CUuid = this->generateUUID(macAddress, "0007");
  m_A2CUuid = this->generateUUID(macAddress, "0008");
  m_A3CUuid = this->generateUUID(macAddress, "0009");
  m_A4CUuid = this->generateUUID(macAddress, "0010");
  m_batteryVoltageUuid = this->generateUUID(macAddress, "0011");
  m_statusCUuid = this->generateUUID(macAddress, "0012");
  m_issueCUuid = this->generateUUID(macAddress, "0013");

  //attach characteristics from UUIDs
  m_LEDC = mp_bot->characteristic(m_LEDCUuid.c_str());
  m_mindControlC = mp_bot->characteristic(m_mindControlCUuid.c_str());
  m_M1C = mp_bot->characteristic(m_M1CUuid.c_str());
  m_M2C = mp_bot->characteristic(m_M2CUuid.c_str());
  m_E1C = mp_bot->characteristic(m_E1CUuid.c_str());
  m_E2C = mp_bot->characteristic(m_E2CUuid.c_str());
  m_A1C = mp_bot->characteristic(m_A1CUuid.c_str());
  m_A2C = mp_bot->characteristic(m_A2CUuid.c_str());
  m_A3C = mp_bot->characteristic(m_A3CUuid.c_str());
  m_A4C = mp_bot->characteristic(m_A4CUuid.c_str());
  m_batteryVoltageC = mp_bot->characteristic(m_batteryVoltageUuid.c_str());
  m_issueC = mp_bot->characteristic(m_issueCUuid.c_str());
  m_statusC = mp_bot->characteristic(m_statusCUuid.c_str());

  Serial.println("$$$$$" + m_name + ": " + String(macAddress.c_str()));
}

Bot::Bot(){
  //dummy constructor
}

void Bot::publishMessageToBot(String characteristic, int value){
  Serial.println(characteristic);

    //write a message to the resepctive bot
    
    if(characteristic == "BUILTIN_LED"){
      if(this->m_LEDC){
        m_LEDC.writeValue(byte(value));
      }
    }else if(characteristic == "mindControl"){
      if(this->m_mindControlC){
        m_mindControlC.writeValue(byte(value));

        //update mind control state to determine which information to send back to central
        if(value){
          Serial.println("MindControl-----------------------------------------");
          m_mindControlState = true;
        }else{
          Serial.println("MindControl Off-------------------------------------------------");
          m_mindControlState = false;
        }
      }
    }else if(characteristic == "M1"){
      if(this->m_M1C){
        m_M1C.writeValue(byte(value));
      }
    }else if(characteristic == "M2"){
      if(this->m_M2C){
        m_M2C.writeValue(byte(value));
      }
    }else if(characteristic == "commandIssue"){
      if(this->m_issueC){
        Serial.print("Attempting to send message on: ");
        Serial.println(m_issueCUuid.c_str());

        m_issueC.writeValue(byte(value));
      }
    }
}

void Bot::cycle(bool isConnected){
    //perform nessecary processes and communication

    sustainConnection(isConnected);

    if(this->m_isConnected){
        //do things
    }
}

void Bot::sustainConnection(bool isConnected){
  //three main steps
  //1. check for connection / update characteristics
  //2. if connection lost attempt to reaqquire
  //3. if failed attempt to reaqquire, try again in a while(loop through for a time)

  if((isConnected == true) && (m_isConnected == true)){
    //this->mp_bot->connected();
    //connection is good        
    return;
  } else {
    //not connected!
    m_isConnected = false;

    Serial.println("Bad Connection");

    //try to evaluate scan at intervals
    if(m_lastScan + m_reattemptConnection < micros() / 1000000.0){

      //TODO:this part of the system needs to be looked at

        Serial.println(this->m_name + " lost connection. Rescanning!");

        //attempt to reaqquire connection
        Serial.println(String(this->m_deviceServiceUUID.c_str()));
        BLE.scanForUuid(String(this->m_deviceServiceUUID.c_str()));
        this->m_lastScan =  micros() / 1000000.0;

        delay(50);

        BLEDevice bot = BLE.available();

        if(bot) {

          Serial.println(bot.address());

            //redo connection
            if(bot.connect()){
                if(bot.discoverAttributes()){
                    mp_bot = &bot;
                    //attach characteristics from UUIDs
                    m_LEDC = mp_bot->characteristic(m_LEDCUuid.c_str());
                    m_mindControlC = mp_bot->characteristic(m_mindControlCUuid.c_str());
                    m_M1C = mp_bot->characteristic(m_M1CUuid.c_str());
                    m_M2C = mp_bot->characteristic(m_M2CUuid.c_str());
                    m_E1C = mp_bot->characteristic(m_E1CUuid.c_str());
                    m_E2C = mp_bot->characteristic(m_E2CUuid.c_str());
                    m_A1C = mp_bot->characteristic(m_A1CUuid.c_str());
                    m_A2C = mp_bot->characteristic(m_A2CUuid.c_str());
                    m_A3C = mp_bot->characteristic(m_A3CUuid.c_str());
                    m_A4C = mp_bot->characteristic(m_A4CUuid.c_str());
                    m_batteryVoltageC = mp_bot->characteristic(m_batteryVoltageUuid.c_str());
                    m_issueC = mp_bot->characteristic(m_issueCUuid.c_str());
                    m_statusC = mp_bot->characteristic(m_statusCUuid.c_str());

                    BLE.stopScan();
                    m_isConnected = true;

                    Serial.println("Found!");

                    return;
                }
            }
        }
    }
  }
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
      + "-0000"
      + "-" + characteristicNumber[0] + characteristicNumber[1] + characteristicNumber[2] + characteristicNumber[3]
      + "-000000000000";
    
    return UUID;
}

bool Bot::isConnected(){
    return this->m_isConnected;
}

std::string Bot::getName(){
  return this->m_name.c_str();
}

std::string Bot::getAddress(){
  return this->mp_bot->address().c_str(); 
}
