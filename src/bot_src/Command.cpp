#include "Command.h"

Command::Command(BLECharacteristic* StatusC, BLECharacteristic* IssueC, RobotContainer* MC, String name){
  //general command structure to run a drawn out task on a bot
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
  //update the command's status and write to controller
  
  if(this->getStatus() != 255){ //don't allow overrite if the command should abort
    m_statusC->writeValue(byte(Status));
  }
}

bool Command::checkForAbort(){
  // check if the command needs to be aborted - command to abort will be sent from controller 
  // TODO - exceptions added later for collisions detected by robot 
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
  //call run to excute command
  Serial.println("Starting to run: " + m_name);

  //sequence on operations with status updates for controller
  this->updateStatus(1);
  this->startup();
  this->updateStatus(2);
  this->superCycle();
  this->updateStatus(3);
  this->cleanup();

  //send final status back based on how command ended
  if(this->checkForAbort() == false){
    this->updateStatus(254);
  }else{
    this->updateStatus(255);
  }

  return;
}

void Command::superCycle(){
  // a wrapper around the cycle function that allows it to run until the command is ended
  while(this->checkForAbort() == false && this->ifEnd() == false){
    cycle();

    //refresh connections
    mp_MC->refreshConnection();
  }
}