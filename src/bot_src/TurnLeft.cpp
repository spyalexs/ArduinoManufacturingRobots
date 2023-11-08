#include "TurnLeft.h"

TurnLeft::TurnLeft(RobotContainer* MC, Communicator* CC, bool preconfirmed):Command(MC, CC, "TurnLeft", preconfirmed){
  //do initialization here
}

void TurnLeft::startup(){
  //do startup tasks 
  m_initialCounts1 = mp_MC->getEncoder1Counts();
  m_initialCounts2 = mp_MC->getEncoder2Counts();
  m_previousTime = mp_MC->getTime();
  m_previousCounts1 = mp_MC->getEncoder1Counts();
  m_previousCounts2 = mp_MC->getEncoder2Counts();

}

void TurnLeft::cycle(){
  //do cycle stuff here
  int counts1 = mp_MC->getEncoder1Counts();
  int counts2 = mp_MC->getEncoder2Counts();
  double time = mp_MC->getTime();

  double instantCPS1 = (counts1 - m_previousCounts1) / (time - m_previousTime);
  double instantCPS2 = -(counts2 - m_previousCounts2) / (time - m_previousTime);

  double vP = .02;
  double vFF = 20 + .015 * this->m_targetCPS;

  int power1 = vP * (m_targetCPS - instantCPS1) + vFF;
  int power2 = vP * (m_targetCPS - instantCPS2) + vFF;
  
  mp_MC->setMotor1(power1);
  mp_MC->setMotor2(-power2);

  // Serial.println("P1: " + String(-power1) + " CPS1: " + String(instantCPS1) + " P2: " + String(power2) + " CPS2: " + String(instantCPS2));


  m_previousTime = time;
  m_previousCounts1 = counts1;
  m_previousCounts2 = counts2;
}

void TurnLeft::cleanup(){
  //do cleanup tasks here

  //ensure motors are told to stop motion
  mp_MC->setMotor1(0);
  mp_MC->setMotor2(0);
}

bool TurnLeft::ifEnd(){
  //return true to stop cycling, false to continue
  int progress = (-(mp_MC->getEncoder2Counts() - this->m_initialCounts2) + (mp_MC->getEncoder1Counts() - this->m_initialCounts1));

  if(progress >= this->m_turningCounts){
    return true;
  }

  return false;
}