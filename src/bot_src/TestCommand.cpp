#include "TestCommand.h"

TestCommand::TestCommand(RobotContainer* MC):Command(MC, "TestCommand"){
  //do initialization here
}

void TestCommand::startup(){
  //do startup tasks here
  this->m_lastToggle = mp_MC->getTime();
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

  return true;
}