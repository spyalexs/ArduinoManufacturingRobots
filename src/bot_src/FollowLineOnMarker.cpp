#include "FollowLineOnMarker.h"

FollowLineOnMarker::FollowLineOnMarker(RobotContainer* MC, Communicator* CC, bool preconfirmed):Command(MC, CC, "FollowLineOnMarker", preconfirmed){
  //do initialization here
}

void FollowLineOnMarker::startup(){
  //do startup tasks here
}

void FollowLineOnMarker::cycle(){
  //do cycle stuff here
  double power1, power2, correction1, correction2;

  mp_MC->velocityControl(&power1, &power2);
  mp_MC->lineControl(&correction1, &correction2);

  mp_MC->setMotor1(power1 + correction1);
  mp_MC->setMotor2(power2 + correction2);
}

void FollowLineOnMarker::cleanup(){
  //do cleanup tasks here

  //ensure motors are told to stop motion
  mp_MC->setMotor1(0);
  mp_MC->setMotor2(0);
}

bool FollowLineOnMarker::ifEnd(){
  //return true to stop cycling, false to continue
  if(!mp_MC->isOnIntersectionMarker()){
    return true;
  }

  return false;
}