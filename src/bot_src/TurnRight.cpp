#include "TurnRight.h"

TurnRight::TurnRight(RobotContainer* MC, Communicator* CC, bool preconfirmed):Command(MC, CC, "TurnRight", preconfirmed){
  //do initialization here
}

void TurnRight::startup(){
  //do startup tasks 
  m_targetCounts = m_turningCounts + mp_MC->getEncoder1Counts();
  m_previousTime = mp_MC->getTime();
  m_previousCounts1 = mp_MC->getEncoder1Counts();
  m_previousCounts2 = mp_MC->getEncoder2Counts();

}

void TurnRight::cycle(){
  //do cycle stuff here
  int counts1 = mp_MC->getEncoder1Counts();
  int counts2 = mp_MC->getEncoder2Counts();
  double time = mp_MC->getTime();

  double instantCPS1 = (counts1 - m_previousCounts1) / (time - m_previousTime);
  double instantCPS2 = -(counts2 - m_previousCounts2) / (time - m_previousTime);

  double vP = .02;
  double vFF = 20 + .015 * this->m_targetCPS;

  int power2 = vP * (m_targetCPS - instantCPS2) + vFF;
  int power1 = vP * (m_targetCPS - instantCPS1) + vFF;

  mp_MC->setMotor1(power1);
  mp_MC->setMotor2(-power2);

  // Serial.println("Power: " + String(power2));
  // Serial.println("ICPS: " + String(instantCPS2));
  // Serial.println(vFF);

  m_previousTime = time;
  m_previousCounts1 = counts1;
  m_previousCounts2 = counts2;
}

void TurnRight::cleanup(){
  //do cleanup tasks here

  //ensure motors are told to stop motion
  mp_MC->setMotor1(0);
  mp_MC->setMotor2(0);
}

bool TurnRight::ifEnd(){
  //return true to stop cycling, false to continue
  if(mp_MC->getEncoder1Counts() >= m_targetCounts){
    return true;
  }

  return false;
}