#include "WaitCommand.h"

WaitCommand::WaitCommand(RobotContainer* MC, Communicator* CC, double waitTime, bool preconfirmed):Command(MC, CC, "WaitCommand", preconfirmed){
  //do initialization here

  this->m_waitTime = 0;
}

void WaitCommand::startup(){
  //do startup tasks here
  this->m_startTime = mp_MC->getTime();
}

void WaitCommand::cycle(){
  //do cycle stuff here
}

void WaitCommand::cleanup(){
  //do cleanup tasks here
}

bool WaitCommand::ifEnd(){
  //return true to stop cycling, false to continue
  if(mp_MC->getTime() < this->m_startTime + this->m_waitTime){
    return false;
  }

  return true;
}