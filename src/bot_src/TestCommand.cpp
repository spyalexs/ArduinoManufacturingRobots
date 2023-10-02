#include "TestCommand.h"

TestCommand::TestCommand(RobotContainer* MC, Communicator* CC, bool preconfirmed):Command(MC, CC, "TestCommand", preconfirmed){
  //do initialization here
}

void TestCommand::startup(){
  //do startup tasks here
  this->m_lastToggle = mp_MC->getTime();

  Serial.println("Starting Test");
  this->m_togglesLeft = 11;
}

void TestCommand::cycle(){
  //do cycle stuff here
  if(mp_MC->getTime() > this->m_lastToggle + 1.0){
    if(this->m_togglesLeft % 2 > 0){
        mp_MC->setLEDStatus(1);
    } else {
        mp_MC->setLEDStatus(0);
    }

    this->m_togglesLeft = this->m_togglesLeft - 1;
    this->m_lastToggle = mp_MC->getTime();
  }
}

void TestCommand::cleanup(){
  //do cleanup tasks here
  mp_MC->setLEDStatus(0);
}

bool TestCommand::ifEnd(){
  //return true to stop cycling, false to continue
  if(this->m_togglesLeft > 0){
    return false;
  }

  Serial.println("Ending Test");

  return true;
}