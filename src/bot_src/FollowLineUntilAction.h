#ifndef followlineuntilaction_h
#define followlineuntilaction_h

#include "Command.h"
#include "RobotContainer.h"

class FollowLineUntilAction : public Command{
  public:
    FollowLineUntilAction(RobotContainer* MC);

    void startup();
    void cycle();
    void cleanup();
    bool ifEnd();

    int m_intersectionCounter;
};


#endif