#include <ArduinoMotorCarrier.h>

int lineFollowerPin = A3;
int intersectionPin = A6;

int intersectionThresholdCycles = 3;
int iCounter = 0;

int lowValue = 50;
int highValue = 100;
double target;

int previousValue = 0;

int targetCPS = 450;

int previousCounts1 = 0;
int previousCounts2 = 0;
int previousCPS1 = 0;
int previousCPS2 = 0;
double previousTime;


void setup(){
  Serial.begin(9600);

  //start up motor controller
  if(!controller.begin()){
    Serial.println("Failed to start motor controller!");
  }

  encoder1.resetCounter(0);
  encoder2.resetCounter(0);

  target = (lowValue + highValue) / 2;

  previousValue = analogRead(A3);

  previousTime = micros() / 1000;
  previousCounts1 = 0;
  previousCounts2 = 0;
}


void loop(){
  //line following stage
  while(iCounter < intersectionThresholdCycles){
    double power1, power2;
    velocityControl(&power1, &power2);

    double correction1, correction2;
    lineControl(&correction1, &correction2);

    Serial.println(iCounter);
    if(!digitalRead(intersectionPin)){
      iCounter += 1;
    } else {
      iCounter = 0;
    }

    M1.setDuty(-(power1 + correction1));
    M2.setDuty((power2 + correction2));
  }

  M1.setDuty(0);
  M2.setDuty(0);

  delay(2000);

  while(!digitalRead(intersectionPin)){
    double power1, power2;
    velocityControl(&power1, &power2);

    M1.setDuty(-(power1));
    M2.setDuty((power2));
  }

  straightForDistance(210);

  turnRight();
  M1.setDuty(0);
  M2.setDuty(0);
  
  iCounter = 0;





}


  void velocityControl(double* Power1, double* Power2){

    delay(20);

    int counts1 = encoder1.getRawCount();
    int counts2 = encoder2.getRawCount();

    double time = double(micros()) / 1000000;
    double instantCPS1 = -(counts1 - previousCounts1) / (time - previousTime);
    double instantCPS2 = (counts2 - previousCounts2) / (time - previousTime);

    double dCPS1 = (instantCPS1 - previousCPS1) / (time - previousTime);
    double dCPS2 = (instantCPS2 - previousCPS2) / (time - previousTime);

    previousTime = time;
    previousCounts1 = counts1;
    previousCounts2 = counts2;
    previousCPS1 = instantCPS1;
    previousCPS2 = instantCPS2;


    double vP = .02;
    double vD = 0;//.000005;
    double vFF = 20;

    double power1 = (vP * (targetCPS - instantCPS1)) + vFF - vD * dCPS1;
    double power2 = (vP * (targetCPS - instantCPS2)) + vFF - vD * dCPS2;

    Serial.println(String(instantCPS1) + "  " + String(dCPS1) + "  " + String(power1));
    Serial.println(String(double(battery.getRaw()) / 236));

    *Power1 = power1;
    *Power2 = power2;
  }

void lineControl(double* correction1, double* correction2){
  double lP = .15;
  double lD = .05;

  int maxCorrection = 25;

  int value = analogRead(lineFollowerPin);
  value = min(highValue, max(lowValue, value));

  double correctionP = lP * (value - target);
  double correctionD = (value - previousValue) * lD;
  int correction = floor(correctionP + correctionD);
  correction = min(correction, maxCorrection);

  previousValue = value;

  *correction1 = correction;
  *correction2 = -correction;
}

void turnRight(){
  bool turning = true;

  int initialCounts2 = getCounts2();
  previousCounts2 = initialCounts2;
  int targetCounts2 = 570 + initialCounts2;

  previousTime = double(micros()) / 1000000;

  while(turning){
    delay(20);

    int counts2 = getCounts2();
    double time = double(micros()) / 1000000;

    double instantCPS2 = (counts2 - previousCounts2) / (time - previousTime);

    double vP = .02;
    double vD = 0;//.000005;
    double vFF = 25;

    int power2 = vP * (targetCPS - instantCPS2) + vFF;
    int power1 = 0;

    setM1Duty(power1);
    setM2Duty(power2);

    previousTime = time;
    previousCounts2 = counts2;

    if(previousCounts2 >= targetCounts2){
      //end turning process (implement more sphositicated control later?)
      turning = false;
    } else {
      Serial.println(previousCounts2 - initialCounts2);
    }
  }
}

void straightForDistance(int counts){
  int initialCounts1 = getCounts1();
  int initialCounts2 = getCounts2();

  double targetCounts = counts + (initialCounts1 + initialCounts2) / 2;
  double power1, power2;

  while((getCounts1() + getCounts2()) / 2 < targetCounts){
    velocityControl(&power1, &power2);
    setM1Duty(power1);
    setM2Duty(power2);
  }

  //end straight process (implement more sphositicated control later?)

  setM1Duty(0);
  setM2Duty(0);
}

int getCounts1(){
  return -encoder1.getRawCount();
}

int getCounts2(){
  return encoder2.getRawCount();
}

void setM1Duty(int duty){
  M1.setDuty(-duty);
}

void setM2Duty(int duty){
  M2.setDuty(duty);
}