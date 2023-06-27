#include "TravelStraight.h"

TravelStraight::TravelStraight(RobotContainer* MC, Communicator* CC, int counts):Command(MC, CC, "TravelStraight"){
  //do initialization here
  m_straightCounts = counts;
}

void TravelStraight::startup(){
  //do startup tasks here
  m_targetCounts = m_straightCounts + (mp_MC->getEncoder1Counts() + mp_MC->getEncoder2Counts()) / 2;
}

void TravelStraight::cycle(){
  //do cycle stuff here
  double power1, power2;

  mp_MC->velocityControl(&power1, &power2);

  mp_MC->setMotor1(power1);
  mp_MC->setMotor2(power2);

  delay(20);
}

void TravelStraight::cleanup(){
  //do cleanup tasks here

  //ensure motors are told to stop motion
  mp_MC->setMotor1(0);
  mp_MC->setMotor2(0);
}

bool TravelStraight::ifEnd(){
  //return true to stop cycling, false to continue
  if((mp_MC->getEncoder1Counts() + mp_MC->getEncoder2Counts()) / 2 >= m_targetCounts){
    return true;
  }

  return false;
}