#include "TEMPLATE.h"

TEMPLATE::TEMPLATE(BLECharacteristic* StatusC, BLECharacteristic* IssueC):Command(StatusC, IssueC, "FollowLine"){
  //do initialization here
}

void TEMPLATE::startup(){
  //do startup tasks here
}

void TEMPLATE::cycle(){
  //do cycle stuff here
}

void TEMPLATE::cleanup(){
  //do cleanup tasks here
}

bool TEMPLATE::ifEnd(){
  //return true to stop cycling, false to continue
}