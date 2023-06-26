#include "FollowLineUntilAction.h"

FollowLineUntilAction::FollowLineUntilAction(RobotContainer* MC):Command(MC, "FollowLineUntilMarker"){
  //do initialization here
  m_intersectionCounter = 0;
}

void FollowLineUntilAction::startup(){
  //do startup tasks here
}

void FollowLineUntilAction::cycle(){
  //do cycle stuff here
  double power1, power2, correction1, correction2;

  mp_MC->velocityControl(&power1, &power2);
  mp_MC->lineControl(&correction1, &correction2);

  mp_MC->setMotor1(power1 + correction1);
  mp_MC->setMotor2(power2 + correction2);


  if(mp_MC->isOnIntersectionMarker()){
    m_intersectionCounter += 1;
  }

  delay(20);
}

void FollowLineUntilAction::cleanup(){
  //do cleanup tasks here

  //ensure motors are told to stop motion
  mp_MC->setMotor1(0);
  mp_MC->setMotor2(0);
}

bool FollowLineUntilAction::ifEnd(){
  //return true to stop cycling, false to continue
  if(m_intersectionCounter >= 3){
    return true;
  }

  return false;
}