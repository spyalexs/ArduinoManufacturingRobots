#include "FollowLineUntilMarker.h"

FollowLineUntilMarker::FollowLineUntilMarker(BLECharacteristic* StatusC, BLECharacteristic* IssueC, MotionController* MC):Command(StatusC, IssueC, "FollowLine"){
  //do initialization here
  mp_MC = MC;
}

void FollowLineUntilMarker::startup(){
  //do startup tasks here
}

void FollowLineUntilMarker::cycle(){
  //do cycle stuff here
  double power1, power2, correction1, correction2;

  mp_MC->velocityControl(&power1, &power2);
  mp_MC->lineControl(&correction1, &correction2);

  Serial.println(power1);
  Serial.println(power2);
  Serial.println(correction1);
  Serial.println(correction2);

  mp_MC->setMotor1(power1 + correction1);
  mp_MC->setMotor2(power2 + correction2);

  delay(20);
}

void FollowLineUntilMarker::cleanup(){
  //do cleanup tasks here
}

bool FollowLineUntilMarker::ifEnd(){
  //return true to stop cycling, false to continue
  return false;
}