#include "TurnLeft.h"

TurnLeft::TurnLeft(BLECharacteristic* StatusC, BLECharacteristic* IssueC, MotionController* MC):Command(StatusC, IssueC, MC, "TurnLeft"){
  //do initialization here
}

void TurnLeft::startup(){
  //do startup tasks 
  m_targetCounts = m_turningCounts + mp_MC->getEncoder1Counts();
  m_previousTime = mp_MC->getTime();
  m_previousCounts = mp_MC->getEncoder1Counts();

}

void TurnLeft::cycle(){
  //do cycle stuff here
  int counts1 = mp_MC->getEncoder1Counts();
  double time = mp_MC->getTime();

  double instantCPS1 = (counts1 - m_previousCounts) / (time - m_previousTime);

  double vP = .02;
  double vFF = 25;

  int power1 = vP * (m_targetCPS - instantCPS1) + vFF;
  int power2 = 0;

  mp_MC->setMotor1(power1);
  mp_MC->setMotor2(power2);

  Serial.println(power1);
  Serial.println(instantCPS1);

  m_previousTime = time;
  m_previousCounts = counts1;
}

void TurnLeft::cleanup(){
  //do cleanup tasks here

  //ensure motors are told to stop motion
  mp_MC->setMotor1(0);
  mp_MC->setMotor2(0);
}

bool TurnLeft::ifEnd(){
  //return true to stop cycling, false to continue
  if(mp_MC->getEncoder1Counts() >= m_targetCounts){
    return true;
  }

  return false;
}