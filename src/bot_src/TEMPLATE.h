#ifndef TEMPLATE_h
#define TEMPLATE_h

#include "Command.h"

class TEMPLATE : public Command{
  public:
    TEMPLATE(RobotContainer* MC, Communicator* CC);

    void startup();
    void cycle();
    void cleanup();
    bool ifEnd();
};


#endif