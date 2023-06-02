#include "RobotContainer.h"

RobotContainer::RobotContainer(mc::DCMotor* motor1, mc::DCMotor* motor2, mc::Encoder* encoder1, mc::Encoder* encoder2, uint8_t lineFollowerPinName, uint8_t intersectionPinName, uint8_t codePinName){
  mp_central = NULL; //leave null for now update later
  m_motor1 = motor1;
  m_motor2 = motor2;
  m_encoder1 = encoder1;
  m_encoder2 = encoder2;
  m_lineFollowerPin = lineFollowerPinName;
  m_intersectionPin = intersectionPinName;
  m_codePin = codePinName;
}

void RobotContainer::setMotor1(int duty){
  this->m_motor1->setDuty(-duty);
}

void RobotContainer::setMotor2(int duty){
  this->m_motor2->setDuty(duty);
}

void RobotContainer::setLEDStatus(int status){
  if(status == 0){
    digitalWrite(LED_BUILTIN, LOW);
  }else{
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

int RobotContainer::getEncoder1Counts(){
  return -this->m_encoder1->getRawCount();
}

int RobotContainer::getEncoder2Counts(){
  return this->m_encoder2->getRawCount();
}

int RobotContainer::getLineFollowerPinReading(){
  return analogRead(m_lineFollowerPin);
}

bool RobotContainer::isOnIntersectionMarker(){
  return !digitalRead(m_intersectionPin);
}

bool RobotContainer::isCodePin(){
  return !digitalRead(m_codePin);
}

double RobotContainer::getTime(){
  return micros() / 1000000.0;
}

double RobotContainer::getBatteryVoltage(){
  return battery.getRaw() / 236.0;

}

bool RobotContainer::refreshConnection(){
  if(mp_central){
    return mp_central->connected();
  }
}

void RobotContainer::setCentralPtr(BLEDevice* central){
  mp_central = central;
}

void RobotContainer::lineControl(double* Correction1, double* Correction2){
  // a in-the-loop pd controller for line following

  //be sure the previous results haven't gone bad
  if(m_linePreviousTime + m_staleTime < this->getTime() || m_linePreviousTime == 0){
    //refresh previous results
    m_linePreviousValue = this->getLineFollowerPinReading();
  }

  double lP = .15;
  double lD = .001;

  int maxCorrection = 25;

  int value = this->getLineFollowerPinReading();
  double time = getTime();

  value = min(m_lineHighValue, max(m_lineLowValue, value));

  double correctionP = lP * (value - (m_lineLowValue + m_lineHighValue) / 2);
  double correctionD = (value - m_linePreviousValue) / (time - m_linePreviousTime) * lD;
  int correction = floor(correctionP + correctionD);
  correction = min(correction, maxCorrection);

  m_linePreviousValue = value;
  m_linePreviousTime = time;

  *Correction1 = correction;
  *Correction2 = -correction;
}

void RobotContainer::velocityControl(double* Power1, double* Power2){
  //an in the loop controller for robot velocity

  //be sure the previous results haven't gone bad
  if(m_velPreviousTime + m_staleTime < this->getTime() || m_velPreviousTime == 0){
    //refresh previous results
    m_velPreviousCounts1 = this->getEncoder1Counts();
    m_velPreviousCounts2 = this->getEncoder2Counts();
    m_velPreviousCPS1 = 0;
    m_velPreviousCPS2 = 0;
  }

  int counts1 = this->getEncoder1Counts();
  int counts2 = this->getEncoder2Counts();

  double time = this->getTime();
  double instantCPS1 = (counts1 - m_velPreviousCounts1) / (time - m_velPreviousTime);
  double instantCPS2 = (counts2 - m_velPreviousCounts2) / (time - m_velPreviousTime);

  double dCPS1 = (instantCPS1 - m_velPreviousCPS1) / (time - m_velPreviousTime);
  double dCPS2 = (instantCPS2 - m_velPreviousCPS2) / (time - m_velPreviousTime);

  m_velPreviousTime = time;
  m_velPreviousCounts1 = counts1;
  m_velPreviousCounts2 = counts2;
  m_velPreviousCPS1 = instantCPS1;
  m_velPreviousCPS2 = instantCPS2;


  double vP = .02;
  double vD = 0;//.000005;
  double vFF = 20;

  double power1 = (vP * (m_velTargetCPS - instantCPS1)) + vFF - vD * dCPS1;
  double power2 = (vP * (m_velTargetCPS - instantCPS2)) + vFF - vD * dCPS2;

  *Power1 = power1;
  *Power2 = power2;
}