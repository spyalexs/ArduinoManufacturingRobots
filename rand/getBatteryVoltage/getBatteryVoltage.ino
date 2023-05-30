#include <ArduinoMotorCarrier.h>


void setup(){
  Serial.begin(9600);

  if(!controller.begin()){
    Serial.println("Failed to bring up controller!");
  }

  //ensure motors are of
  M1.setDuty(0);
  M2.setDuty(0);
}

void loop(){

  delay(500);
  Serial.print("Idle voltage: ");
  Serial.println(double(battery.getRaw() / 236.0));

  Serial.println("Please unplug the usb connector: this will test the battery voltage while not attached to outside power!");
  delay(10000);
  Serial.println("If you see this message, your too late try again next time");
  double raw = battery.getRaw() / 236.0;
  delay(10000);
  Serial.print("Disconnected voltage: ");
  Serial.println(raw);
}