#ifndef WAITCOMMAND_H
#define WAITCOMMAND_H

#include "Command.h"

class WaitCommand : public Command{
  public:
    WaitCommand(RobotContainer* MC, Communicator* CC, double waitTime, bool preconfirmed = false);

    void startup();
    void cycle();
    void cleanup();
    bool ifEnd();

    double m_waitTime;
    double m_startTime;
};


#endif