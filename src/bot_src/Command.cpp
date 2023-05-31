#include "Command.h"

Command::Command(BLECharacteristic* StatusC, BLECharacteristic* IssueC, MotionController* MC, String name){
    m_statusC = StatusC;
    m_issueC = IssueC;
    m_name = name;
    mp_MC = MC;
}

int Command::getStatus(){
  //read the command status

    byte value = 0;
    m_statusC->readValue(value);

    return int(value);
}

void Command::updateStatus(int Status){
  if(this->getStatus() != 255){ //don't allow overrite if the command should abort
    m_statusC->writeValue(byte(Status));
  }
}

bool Command::checkForAbort(){
  // check if the command needs to be aborted
  byte value = 0;
  m_issueC->readValue(value);

  if(int(value) == 255){
    return true;
  }

  return false;
}

void Command::startup(){
  // this should be overriden with tasks when starting command
  return;
}

void Command::cycle(){
  // this should be overriden to execute the commands cycle
  return;
}

bool Command::ifEnd(){
  //this shoud be ovverride to return True when the cycling should be stopped
  return false;
}

void Command::cleanup(){
  //this should be overriden with tasks when ending command
}

void Command::run(){
  Serial.println("Starting to run: " + m_name);

  this->updateStatus(1);
  this->startup();
  this->updateStatus(2);
  this->superCycle();
  this->updateStatus(3);
  this->cleanup();

  if(this->checkForAbort() == false){
    this->updateStatus(254);
  }else{
    this->updateStatus(255);
  }

  return;
}

void Command::superCycle(){
  while(this->checkForAbort() == false && this->ifEnd() == false){
    cycle();

    //refresh connections
    mp_MC->refreshConnection();
  }
}