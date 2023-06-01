#ifndef TEMPLATE_h
#define TEMPLATE_h

#include "Command.h"

class TEMPLATE : public Command{
  public:
    TEMPLATE(BLECharacteristic* StatusC, BLECharacteristic* IssueC, RobotContainer* MC);

    void startup();
    void cycle();
    void cleanup();
    bool ifEnd();
};


#endif