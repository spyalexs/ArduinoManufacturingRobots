#include "StationContainer.h"

StationContainer::StationContainer(){
  m_display = Display();

  pinMode(LED_BUILTIN, OUTPUT);

}

void StationContainer::setLEDStatus(int status){
  if(status == 0){
    digitalWrite(LED_BUILTIN, LOW);
  }else{
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

double StationContainer::getTime(){
  return micros() / 1000000.0;
}

double StationContainer::getBatteryVoltage(){
  double voltage = battery.getRaw() / 77.0;

  //update battery icon - deadzone between hi and low to prevent many icon requests
  if(voltage < 1){
    //usb power
    if(this->m_powerState != 0){
      this->m_powerState = 0;
      Serial.println("Adding Battery job");
      this->m_display.addIconDrawJob(11, "power");
    }
  } else if(this->m_powerState == 0){
    //usb to battery power
    if(voltage > 7){
      this->m_powerState = 2;
      this->m_display.addIconDrawJob(11, "bathi");
    } else {
      this->m_powerState = 1;
      this->m_display.addIconDrawJob(11, "batlow");
    }
  } else if(this->m_powerState == 1){
    //low to high
    if(voltage > 7.1){
      this->m_powerState = 1;
      this->m_display.addIconDrawJob(11, "bathi");
    }
  }else{
    //high to low
    if(voltage < 6.9){
      this->m_powerState = 1;
      this->m_display.addIconDrawJob(11, "batlow");
    }
  }
  return voltage;
}

void StationContainer::cycle(){

  this->m_display.cycle();

}