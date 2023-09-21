#ifndef motioncontroller_h
#define motioncontroller_h

#include <ArduinoMotorCarrier.h>
#include <Adafruit_MCP23X17.h>
#include "Display.h"
#include "Arduino.h"

#define CONTAINER_ULTRASONIC_ACTIVE false
#define CONTAINER_ENCODER_CLICK_MIN_TIME 1000 //ms

// a wrapper class around motion, allows for the easy utilization of subroutines and control methods

class RobotContainer{
  private:
    //encoder
    bool m_encoderDirection = true; //true: CW,  false:: CCW
    bool m_encoderLastPosition = true; //both high true, both low false
    int m_encoderCounts = 0; //counts are even on low and high on odd
    bool m_encoderReleased = true;
    bool m_encoderClickSent = false;
    long m_encoderClickStart = 0.0;

    void cycleEncoder();

    void handleMenuSelection();

    uint8_t m_cycleCounter = 0; // counter for cycle - switch between cycles
  public:
    RobotContainer(mc::DCMotor* motor1, mc::DCMotor* motor2, mc::Encoder* encoder1, mc::Encoder* encoder2, uint8_t lineFollowerPinName, uint8_t intersectionPinName, uint8_t codePinName);

    //define robot periphrals
    mc::DCMotor* m_motor1;
    mc::DCMotor* m_motor2;

    mc::Encoder* m_encoder1;
    mc::Encoder* m_encoder2;

    uint8_t m_lineFollowerPin;
    uint8_t m_intersectionPin;
    uint8_t m_codePin;

    uint8_t m_encoderClickPin = IN4;
    uint8_t m_encoderCWFPin = IN4;
    uint8_t m_encoderCCWFPin = IN2;

    uint8_t m_ultrasonicTriggerPin = 11;
    uint8_t m_ultrasonicEchoPin = 12;
    
    //robot display
    Display m_display;

    Adafruit_MCP23X17* mp_gpio = nullptr;

    //class-wide constants
    double m_staleTime = 0.1; // the time it takes for control results to need to be refreshed

    //define line control varibles
    int m_lineHighValue = 100;
    int m_lineLowValue = 50;
    int m_linePreviousValue = 0;
    double m_linePreviousTime = 0;

    uint8_t m_powerState = 99;
    //powerstates:
    //  0 - usb or charging
    //  1 - bat low
    //  2 - bat high

    //define velocity control varibles
    int m_velTargetCPS = 450;
    int m_velPreviousCounts1 = 0;
    int m_velPreviousCounts2 = 0;
    double m_velPreviousCPS1 = 0;
    double m_velPreviousCPS2 = 0;
    double m_velPreviousTime = 0;

    double m_ultrasonicDistance = -1;
    
    //getter and setter functions
    void setMotor1(int duty);
    void setMotor2(int duty);

    void setLEDStatus(int status);

    int getEncoder1Counts();
    int getEncoder2Counts();

    int getLineFollowerPinReading();
    bool isOnIntersectionMarker();
    bool isCodePin();

    double getTime();
    double getBatteryVoltage();
  
    //control functions
    void lineControl(double* Corrrection1, double* Correction2);
    void velocityControl(double* Power1, double* Power2);

    //encoder
    bool isEncoderClicked();
    int getDisplayEncoderCounts();
    void resetDisplayEncoder();
    void handleEncoderClick();

    //ultrasonic sensor
    double getDistance();
    void cycleUltrasonic();

    void cycle();

    //gpio
    void setGPIOPointer(Adafruit_MCP23X17* gpio);

    void BypassEncoder();
};


#endif