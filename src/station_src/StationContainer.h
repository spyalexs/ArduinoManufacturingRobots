#ifndef motioncontroller_h
#define motioncontroller_h

#include <ArduinoMotorCarrier.h>
#include "Display.h"

#define CONTAINER_ULTRASONIC_ACTIVE false
#define CONTAINER_ENCODER_CLICK_MIN_TIME 500 //ms

// a wrapper class around motion, allows for the easy utilization of subroutines and control methods

class StationContainer{
  private:

  uint8_t m_powerState = 255;
  public:
    StationContainer();

    //define robot periphrals
    mc::DCMotor* m_motor1;
    mc::DCMotor* m_motor2;

    mc::Encoder* m_encoder1;
    mc::Encoder* m_encoder2;

    uint8_t m_lineFollowerPin;
    uint8_t m_intersectionPin;
    uint8_t m_codePin;

    uint8_t m_encoderClickPin = 11;
    uint8_t m_encoderCWFPin = 12;
    uint8_t m_encoderCCWFPin = 12;

    uint8_t m_ultrasonicTriggerPin = 11;
    uint8_t m_ultrasonicEchoPin = 12;
    
    //robot display
    Display m_display;

    //class-wide constants
    double m_staleTime = 0.1; // the time it takes for control results to need to be refreshed

    void setLEDStatus(int status);

    double getTime();
    double getBatteryVoltage();
  
    void cycle();
};


#endif