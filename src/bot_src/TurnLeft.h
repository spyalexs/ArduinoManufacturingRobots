#ifndef turnleft_h
#define turnleft_h

#include "Command.h"
#include "RobotContainer.h"

class TurnLeft : public Command{
  public:
    TurnLeft(RobotContainer* MC);

    void startup();
    void cycle();
    void cleanup();
    bool ifEnd();

    int m_turningCounts = 570; // the amount of counts the robot should turn to
    int m_targetCPS = 450;
    int m_targetCounts = 0; // initial + turning

    int m_previousCounts = 0;
    double m_previousTime = 0;
};


#endif