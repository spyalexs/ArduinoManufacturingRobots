#ifndef turnright_h
#define turnright_h

#include "Command.h"
#include "RobotContainer.h"

class TurnRight : public Command{
  public:
    TurnRight(RobotContainer* MC, Communicator* CC);

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