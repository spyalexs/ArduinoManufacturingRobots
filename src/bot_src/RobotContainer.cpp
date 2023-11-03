#include "RobotContainer.h"

RobotContainer::RobotContainer(mc::DCMotor* motor1, mc::DCMotor* motor2, mc::Encoder* encoder1, mc::Encoder* encoder2, uint8_t lineFollowerPinName, uint8_t intersectionPinName, uint8_t codePinName){
  m_motor1 = motor1;
  m_motor2 = motor2;
  m_encoder1 = encoder1;
  m_encoder2 = encoder2;
  m_lineFollowerPin = lineFollowerPinName;
  m_intersectionPin = intersectionPinName;
  m_codePin = codePinName;
  m_display = Display();

  pinMode(LED_BUILTIN, OUTPUT);

  //configure ultrasonic pins

  if(CONTAINER_ULTRASONIC_ACTIVE){
    pinMode(this->m_ultrasonicTriggerPin, OUTPUT); // change to OUTPUT when ultrasonic connect, INPUT for encoder
  } else {
    pinMode(this->m_ultrasonicTriggerPin, INPUT);
  }

  pinMode(this->m_ultrasonicEchoPin, INPUT);
  pinMode(this->m_encoderCWFPin, INPUT);
  pinMode(this->m_encoderCCWFPin, INPUT);

  pinMode(1, INPUT);
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
  return this->m_encoder1->getRawCount();
}

int RobotContainer::getEncoder2Counts(){
  return -this->m_encoder2->getRawCount();
}

uint16_t RobotContainer::getLineFollowerPinReading(){
  return analogRead(m_lineFollowerPin);
}

uint16_t RobotContainer::getMarkerPinReading(){
  return analogRead(m_intersectionPin);
}

bool RobotContainer::isOnIntersectionMarker(){
  if(analogRead(m_intersectionPin) > this->m_intersectionMarkerThreshold){
    return false;
  } 

  return true;
}

bool RobotContainer::isCodePin(){
  return !digitalRead(m_codePin);
}

double RobotContainer::getTime(){
  return micros() / 1000000.0;
}

double RobotContainer::getBatteryVoltage(){
  double voltage = battery.getRaw() / 77.0;

  //update battery icon - deadzone between hi and low to prevent many icon requests
  if(voltage < 1){
    //usb power
    if(this->m_powerState != 0){
      this->m_powerState = 0;
      this->m_display.addIconDrawJob(11, "power");
    }
  } else if(this->m_powerState == 0){
    //usb to battery power
    if(voltage > 7.55){
      this->m_powerState = 2;
      this->m_display.addIconDrawJob(11, "bathi");
    } else {
      this->m_powerState = 1;
      this->m_display.addIconDrawJob(11, "batlow");
    }
  } else if(this->m_powerState == 1){
    //low to high
    if(voltage > 7.6){
      this->m_powerState = 1;
      this->m_display.addIconDrawJob(11, "bathi");
    }
  }else{
    //high to low
    if(voltage < 7.5){
      this->m_powerState = 1;
      this->m_display.addIconDrawJob(11, "batlow");
    }
  }

  return voltage;
}

void RobotContainer::lineControl(double* Correction1, double* Correction2){
  // a in-the-loop pd controller for line following

  //be sure the previous results haven't gone bad
  if(m_linePreviousTime + m_staleTime < this->getTime() || m_linePreviousTime == 0){
    //refresh previous results
    m_linePreviousValue = this->getLineFollowerPinReading();
  }

  double lP = 1.05;
  double lD = .02;

  int maxCorrection = 25;

  int value = this->getLineFollowerPinReading();
  double time = getTime();

  value = std::min(m_lineHighValue, std::max(m_lineLowValue, value));

  double correctionP = lP * (value - (m_lineLowValue + m_lineHighValue) / 2);
  double correctionD = (value - m_linePreviousValue) / (time - m_linePreviousTime) * lD;
  int correction = floor(correctionP + correctionD);
  correction = std::min(correction, maxCorrection);

  m_linePreviousValue = value;
  m_linePreviousTime = time;
  *Correction1 = -correction;
  *Correction2 = +correction;
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

  double vP = .04;
  double vD = 0;//.000005;
  double vFF = 20 + .011 * this->m_velTargetCPS;

  double power1 = (vP * (m_velTargetCPS - instantCPS1)) + vFF - vD * dCPS1;
  double power2 = (vP * (m_velTargetCPS - instantCPS2)) + vFF - vD * dCPS2;

  *Power1 = power1;
  *Power2 = power2;

  // Serial.println("CPS: " + String(instantCPS1) + " Power: " + String(power1));
}

bool RobotContainer::isEncoderClicked(){

  Serial.println(digitalRead(this->m_encoderClickPin));

  if(!digitalRead(this->m_encoderClickPin)){
    //if a click signal is being sent

    if(this->m_encoderReleased){
      //start timer - tune out false signals
      this->m_encoderReleased = false;

      this->m_encoderClickStart = millis();
    }

  } else{
    //encoder click signal stopped so reset logic
    this->m_encoderReleased = true;
    this->m_encoderClickSent = false;
  }

  if(m_encoderClickStart + CONTAINER_ENCODER_CLICK_MIN_TIME < millis() && this->m_encoderClickSent == false && this->m_encoderReleased == false){
    //the encoder has been pressed for the right time and the signal hasn't been sent
    this->m_encoderClickSent = true;
    
    return false;
  }

  return false;

}

void RobotContainer::cycleEncoder(){
  //this works because of low cycle time
  
  //check to see if a direction signal is being sent
  bool ccw = digitalRead(this->m_encoderCCWFPin);  
  bool cw = digitalRead(this->m_encoderCWFPin);  
  
  //2 signal mode
  if(ccw != cw){
    //direction signal
    
    //check against previous state
    if(this->m_encoderLastPosition == ccw){
      //if cw has changed moving in cw direction
      m_encoderDirection = true;
    } else{
      //moving in ccw direction
      m_encoderDirection = false;
    }

    return;
  } 

  //use when switch is disconnnected
  //"click"
  if(ccw != this->m_encoderLastPosition){
    this->m_encoderClickStart = millis();
    this->m_encoderReleased = true;
  }

  if(this->m_encoderClickStart + 5000 < millis() && this->m_encoderReleased){
    this->m_encoderReleased = false;
    
    handleEncoderClick();
  }

  if(ccw != this->m_encoderLastPosition){
    //if state has changed

    //set signal 
    this->m_encoderLastPosition = ccw;

    //increment counts
    if(this->m_encoderDirection){
      this->m_encoderCounts++;
    } else {
      this->m_encoderCounts--;
    }
  }

  //cycle / handle clicks - use when switch is working
  // if(this->isEncoderClicked()){
  //   Serial.println("Clicked");
  //   handleEncoderClick();
  // }

  // 1 Signal Mode - only one side of the dial attached
  // if(ccw != this->m_encoderLastPosition){
  //   this->m_encoderCounts++;

  //   this->m_encoderLastPosition = ccw;
  // }
}

int RobotContainer::getDisplayEncoderCounts(){
  return this->m_encoderCounts;
}

void RobotContainer::resetDisplayEncoder(){
  this->m_encoderCounts = 0;
}

void RobotContainer::cycleUltrasonic(){
  //cycle the ultrasonic sensor

  //ensure trigger is low for some time - it should be by default
  digitalWrite(this->m_ultrasonicTriggerPin, LOW);
  delayMicroseconds(5);

  //send pulse
  digitalWrite(this->m_ultrasonicTriggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(this->m_ultrasonicTriggerPin, LOW);

  //wait until pulse has finished sending
  while(!digitalRead(this->m_ultrasonicEchoPin)){
  }

  long pingSent = micros(); // pulse timer
  bool recievedPulse = false;
  
  long time = 0;

  //delay while waiting for pulse to return / timeout to occur
  while(!recievedPulse && time < pingSent + 5000){
    //update time
    time = micros();

    if(!digitalRead(m_ultrasonicEchoPin)){
      //mark if a pulse was recieved 
      recievedPulse = true;
    }
  }

  if(recievedPulse){
    //calculate distance from time
    //dist = time / (343 * 2 * 1000000)
    this->m_ultrasonicDistance = 343 * (time - pingSent) / (2.0 * 1000000);
  } else {
  // set distance to negative 1 to signal no objects within proximity
    this->m_ultrasonicDistance = -1;
  }
}

double RobotContainer::getDistance(){
  //return the last distance reading from the ultrasonic sensor

  return this->m_ultrasonicDistance;
}

void RobotContainer::cycle(){
  //cycle encoder
  this->cycleEncoder();

  //if this becomes a problem -- it should be possible to integrate ultrasonic cycle into display cycle - will just be needlessly complex
  if(this->m_cycleCounter < 25){
    //cycle display
    this->m_display.cycle(this->getDisplayEncoderCounts());

    //if the display requested that the menu page be refreshed
    if(this->m_display.m_refreshMenu == true){
      //get sensor data
      SensorData sd = SensorData(this->getLineFollowerPinReading(), this->getMarkerPinReading(), 0, 0);

      this->m_display.refreshMenuPage(sd);
    }

  } else {
    this->m_cycleCounter = 0;

    //check to ensure there is an ultrasonic before cycling -- otherwise causes hangups
    if(CONTAINER_ULTRASONIC_ACTIVE){
      //cycle ultrasonic instead of display every 25 times
      this->cycleUltrasonic();
    } else {
      this->m_display.cycle(this->getDisplayEncoderCounts());
    }
  }

  this->m_cycleCounter++;
}

void RobotContainer::handleMenuSelection(){
  //handle a selection in a menu
  if(this->m_display.getSelectedMenuItem() >=0){
    //if a menu item has been selected
    Serial.println(this->m_display.getMenuItemPointer(this->m_display.getSelectedMenuItem())->m_text);
  }
}

void RobotContainer::handleEncoderClick(){
  uint8_t item = this->m_display.getSelectedMenuItem();


  if(item != 255){
    //if a menu item has been selected get its value to determine course of action

    switch(this->m_display.getMenuItemPointer(item)->m_value){
      case 1:
        //move from start menu to running ui

        //clear old
        this->m_display.addWipeDisplayJob(false, ILI9341_BLACK);

        //reset encoder counts so the highlight begins off
        this->resetDisplayEncoder();

        //disable menu
        this->m_display.disableMenu();

        break;
      case 2:
        //move from start menu to test menu

        //clear old
        this->m_display.addWipeDisplayJob(false, ILI9341_BLACK);

        //reset encoder counts so the highlight begins off
        this->resetDisplayEncoder();

        //draw test menu
        this->m_display.drawTestingMenu();
        break;

      case 3:
        //move back a testing menu page

        //wipe old page
        this->m_display.addWipeDisplayJob(false, ILI9341_BLACK);

        //reset encoder counts so the highlight begins off
        this->resetDisplayEncoder();

        if(this->m_display.getPreviousTestingMenuPage() == 0){
          //if exit test menu page, draw main menu
          this->m_display.drawOpeningMenu();
        } else {
          //draw previous testing menu page
          this->m_display.drawTestingMenu(this->m_display.getPreviousTestingMenuPage());
        }

        break;

      case 4:
        //move up a testing menu page

        //wipe old page
        this->m_display.addWipeDisplayJob(false, ILI9341_BLACK);

        //reset encoder counts so the highlight begins off
        this->resetDisplayEncoder();

        if(this->m_display.getNextTestingMenuPage() == 0){
          //if exit test menu page, draw main menu
          this->m_display.drawOpeningMenu();
        } else {
          //draw previous testing menu page
          this->m_display.drawTestingMenu(this->m_display.getNextTestingMenuPage());
        }

        break;

      default:
        Serial.println("Not Ready for that action yet");
    }
    
  }
}

void RobotContainer::setGPIOPointer(Adafruit_MCP23X17* gpio){
  //set the pointer to the gpio board
  this->mp_gpio = gpio;


  //set all pins to be input
  for(int i = 0; i < 16; i++){
    mp_gpio->pinMode(0, INPUT);
  }

  mp_gpio->pinMode(1, OUTPUT);
  mp_gpio->digitalWrite(1, HIGH);
}

void RobotContainer::BypassEncoder(){
  
  //clear old
  this->m_display.addWipeDisplayJob(false, ILI9341_BLACK);

  //reset encoder counts so the highlight begins off
  this->resetDisplayEncoder();

  //disable menu
  this->m_display.disableMenu();

  //draw in icon holders
  this->m_display.setIconsCount(6);
}

void RobotContainer::setConnectionLight(bool connected){
  //write to tell the gpio board to turn the light on when connected
  Serial.println("Writing");
  this->mp_gpio->digitalWrite(0,HIGH);
}
