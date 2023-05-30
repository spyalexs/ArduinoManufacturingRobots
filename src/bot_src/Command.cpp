#include "Command.h"

Command::Command(BLECharacteristic* StatusC, BLECharacteristic* IssueC){
    m_statusC = StatusC;
    m_issueC = IssueC;
}

int Command::getStatus(){
  //read the command status

    byte value = 0;
    m_statusC->readValue(value);

    return int(value);
}

void Command::updateStatus(int Status){
  if(this->getStatus() != 255){ //don't allow overrite if the command should abort
    m_statusC->writeValue(Status);
  }
}

bool Command::checkForAbort(){
  // check if the command needs to be aborted
  byte value = 0;
  m_issueC->readValue(value);

  if(value == 255){
    return true;
  }

  return false;
}

void startup(){
  // this should be overriden with tasks when starting command
  return;
}

void cycle(){
  // this should be overriden to execute the commands cycle
  return;
}

bool ifEnd(){
  //this shoud be ovverride to return True when the cycling should be stopped
  return false;
}

void cleanup(){
  //this should be overriden with tasks when ending command
}

void run(){
  this->updateStatus(1);
  this->startup();
  this->updateStatus(2);
  this->superCycle();
  this->updateStatus(3);
  this->cleanup();

  if(this->checkForAbort == false){
    this->updateStatus(254);
  }else{
    this->updateStatus(255);
  }

  return;
}

void superCycle(){
  while(this->checkForAbort == false && this->checkForAbort == false){
    cycle();
  }
}