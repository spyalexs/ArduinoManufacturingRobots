#ifndef motioncontroller_h
#define motioncontroller_h

#include <ArduinoMotorCarrier.h>
#include <ArduinoBLE.h>

// a wrapper class around motion, allows for the easy utilization of subroutines and control methods

class RobotContainer{
  public:
    RobotContainer(mc::DCMotor* motor1, mc::DCMotor* motor2, mc::Encoder* encoder1, mc::Encoder* encoder2, uint8_t lineFollowerPinName, uint8_t intersectionPinName, uint8_t codePinName);

    //define robot periphrals
    mc::DCMotor* m_motor1;
    mc::DCMotor* m_motor2;

    mc::Encoder* m_encoder1;
    mc::Encoder* m_encoder2;

    BLEDevice* mp_central;

    uint8_t m_lineFollowerPin;
    uint8_t m_intersectionPin;
    uint8_t m_codePin;

    //class-wide constants
    double m_staleTime = 0.1; // the time it takes for control results to need to be refreshed

    //define line control varibles
    int m_lineHighValue = 100;
    int m_lineLowValue = 50;
    int m_linePreviousValue = 0;
    double m_linePreviousTime = 0;

    //define velocity control varibles
    int m_velTargetCPS = 450;
    int m_velPreviousCounts1 = 0;
    int m_velPreviousCounts2 = 0;
    double m_velPreviousCPS1 = 0;
    double m_velPreviousCPS2 = 0;
    double m_velPreviousTime = 0;

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
  
    bool refreshConnection();
    void setCentralPtr(BLEDevice* central);

    //control functions
    void lineControl(double* Corrrection1, double* Correction2);
    void velocityControl(double* Power1, double* Power2);

};


#endif