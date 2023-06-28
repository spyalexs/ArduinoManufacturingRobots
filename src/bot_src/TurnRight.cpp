#include "TurnRight.h"

TurnRight::TurnRight(RobotContainer* MC, Communicator* CC):Command(MC, CC, "TurnRight"){
  //do initialization here
}

void TurnRight::startup(){
  //do startup tasks 
  m_targetCounts = m_turningCounts + mp_MC->getEncoder2Counts();
  m_previousTime = mp_MC->getTime();
  m_previousCounts = mp_MC->getEncoder2Counts();

}

void TurnRight::cycle(){
  //do cycle stuff here
  int counts2 = mp_MC->getEncoder2Counts();
  double time = mp_MC->getTime();

  double instantCPS2 = (counts2 - m_previousCounts) / (time - m_previousTime);

  double vP = .02;
  double vFF = 25;

  int power2 = vP * (m_targetCPS - instantCPS2) + vFF;
  int power1 = 0;

  mp_MC->setMotor1(power1);
  mp_MC->setMotor2(power2);

  Serial.println(power2);
  Serial.println(instantCPS2);

  m_previousTime = time;
  m_previousCounts = counts2;
}

void TurnRight::cleanup(){
  //do cleanup tasks here

  //ensure motors are told to stop motion
  mp_MC->setMotor1(0);
  mp_MC->setMotor2(0);
}

bool TurnRight::ifEnd(){
  //return true to stop cycling, false to continue
  if(mp_MC->getEncoder2Counts() >= m_targetCounts){
    return true;
  }

  return false;
}