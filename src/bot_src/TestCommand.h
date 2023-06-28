#ifndef TEMPLATE_h
#define TEMPLATE_h

#include "Command.h"

class TestCommand : public Command{
  public:
    TestCommand(RobotContainer* MC, Communicator* CC);

    void startup();
    void cycle();
    void cleanup();
    bool ifEnd();

    double m_lastToggle = 0;
    int m_togglesLeft = 11;
};


#endif