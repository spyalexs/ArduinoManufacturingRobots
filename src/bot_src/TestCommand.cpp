#include "TestCommand.h"

TestCommand::TestCommand(RobotContainer* MC, Communicator* CC):Command(MC, CC, "TestCommand"){
  //do initialization here

  Serial.println("Here");
}

void TestCommand::startup(){
  //do startup tasks here
  this->m_lastToggle = mp_MC->getTime();

  Serial.println("Starting Test");
}

void TestCommand::cycle(){
  //do cycle stuff here
  
  if(mp_MC->getTime() > m_lastToggle + 1.0){
    if(m_togglesLeft % 2 > 0){
        mp_MC->setLEDStatus(1);
    } else {
        mp_MC->setLEDStatus(0);
    }

    m_togglesLeft = m_togglesLeft - 1;
    m_lastToggle = mp_MC->getTime();

    Serial.println("Cycling Test");
  }
}

void TestCommand::cleanup(){
  //do cleanup tasks here
  mp_MC->setLEDStatus(0);
}

bool TestCommand::ifEnd(){
  //return true to stop cycling, false to continue
  if(m_lastToggle > 0){
    return false;
  }

  Serial.println("Ending Test");

  return true;
}