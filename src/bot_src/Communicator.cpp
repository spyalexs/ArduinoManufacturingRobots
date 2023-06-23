#include "Communicator.h"

Communicator::Communicator(const char* issueUUID, const char* statusUUID, const char* LEDUUID):
  m_issueC(issueUUID, BLERead | BLEWrite,0),
  m_statusC(statusUUID, BLERead | BLEWrite, 0),
  m_LEDC(LEDUUID, BLERead | BLEWrite, 0){
  
  Serial.println("Here");
    
  this->m_intialized= false;
}

void Communicator::prepareCharacteristics(){
  
  //Uuid for connection service
  std::string deviceServiceUUID = generateUUID(BLE.address().c_str(), "0000");

  //this is rediculous but i spent like 4 hours trying to figure out a better way and I got nothin
  //string.h steals all the memory :( - no strcpy
  static const char* deviceServiceCUUID = new const char[36] {deviceServiceUUID[0], deviceServiceUUID[1], deviceServiceUUID[2], deviceServiceUUID[3], deviceServiceUUID[4], deviceServiceUUID[5], deviceServiceUUID[6], deviceServiceUUID[7], 
    '-', deviceServiceUUID[9], deviceServiceUUID[10], deviceServiceUUID[11], deviceServiceUUID[12], 
    '-', deviceServiceUUID[14], deviceServiceUUID[15], deviceServiceUUID[16], deviceServiceUUID[17],
    '-', deviceServiceUUID[19], deviceServiceUUID[20], deviceServiceUUID[21], deviceServiceUUID[22], 
    '-', deviceServiceUUID[24], deviceServiceUUID[25], deviceServiceUUID[26], deviceServiceUUID[27], deviceServiceUUID[28], deviceServiceUUID[29], deviceServiceUUID[30], deviceServiceUUID[31], deviceServiceUUID[32], deviceServiceUUID[33], deviceServiceUUID[34], deviceServiceUUID[35]};
  *((char *)deviceServiceCUUID + 36) = '\0';

    BLEService motorCarrierService = BLEService(deviceServiceCUUID); 

    //set local name (bluetooth) display name and advertise service
    BLE.setLocalName("TestBot");
    BLE.setAdvertisedService(motorCarrierService);
    
    Serial.println("1");

    Serial.println("2");

    motorCarrierService.addCharacteristic(m_statusC);
    motorCarrierService.addCharacteristic(m_issueC);
    motorCarrierService.addCharacteristic(m_LEDC);

    Serial.println("3");

    BLE.addService(motorCarrierService);

    Serial.println("4");

    m_statusC.writeValue(byte(0));
    m_issueC.writeValue(byte(0));
    m_LEDC.writeValue(byte(0));

    //advertise the bot on the bluetooth network
    BLE.advertise();
    Serial.println(5);

    this->m_intialized = true;

    Serial.println(m_issueC.uuid());
    Serial.println(this->GetIssueCPointer()->uuid());

}

void Communicator::writeStatus(int status){
    if(this->m_intialized){
        this->m_statusC.writeValue(byte(status));
    }
}

void Communicator::writeIssue(int issue){
    if(this->m_intialized){
        this->m_issueC.writeValue(byte(issue));
    }
}

void Communicator::writeLED(int statusLED){
    if(this->m_intialized){
        this->m_LEDC.writeValue(byte(statusLED));
    }
}

int Communicator::readStatus(){
    if(this->m_intialized){
        byte value;
        this->m_statusC.readValue(value);

        return int(value);
    }

    return -1;
}

int Communicator::readIssue(){
    if(this->m_intialized){
        byte value;
        this->m_issueC.readValue(value);

        return int(value);
    }

    return -1;
}

int Communicator::readLED(){
    if(this->m_intialized){
        byte value;
        this->m_LEDC.readValue(value);

        return int(value);
    }

    return -1;
}

BLECharacteristic* Communicator::GetStatusCPointer(){
    if(this->m_intialized){
      return &(this->m_statusC);
    }

    return nullptr;
}

BLECharacteristic* Communicator::GetIssueCPointer(){
    if(this->m_intialized){
      return &(this->m_issueC);
    }

    return nullptr;
}

BLECharacteristic* Communicator::GetLEDCPointer(){
    if(this->m_intialized){
      return &(this->m_LEDC);
    }

    return nullptr;
}

std::string Communicator::generateUUID(std::string MACAddress, std::string characteristicNumber){
  
  //generatee the UUIDs for the bot's characteristics
  std::string MAP[12];

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