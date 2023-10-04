#ifndef turnright_h
#define turnright_h

#include "Command.h"
#include "RobotContainer.h"

class TurnRight : public Command{
  public:
    TurnRight(RobotContainer* MC, Communicator* CC, bool preconfirmed = false);

    void startup();
    void cycle();
    void cleanup();
    bool ifEnd();

    int m_turningCounts = 4070; // the amount of counts the robot should turn to
    int m_targetCPS = 1400;
    int m_initialCounts1 = 0; // initial reading of encoder 1
    int m_initialCounts2 = 0; // initial reading of encoder 2

    int m_previousCounts1 = 0;
    int m_previousCounts2 = 0;
    double m_previousTime = 0;
};


#endif