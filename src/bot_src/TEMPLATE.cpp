#include "TEMPLATE.h"

TEMPLATE::TEMPLATE(BLECharacteristic* StatusC, BLECharacteristic* IssueC, RobotContainer* MC):Command(StatusC, IssueC, MC, "TEMPLATE"){
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