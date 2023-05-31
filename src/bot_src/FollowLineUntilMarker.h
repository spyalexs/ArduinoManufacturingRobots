#ifndef followlineuntilmarker_h
#define followlineuntilmarker_h

#include "Command.h"
#include "MotionController.h"

class FollowLineUntilMarker : public Command{
  public:
    FollowLineUntilMarker(BLECharacteristic* StatusC, BLECharacteristic* IssueC, MotionController* MC);

    void startup();
    void cycle();
    void cleanup();
    bool ifEnd();

    int m_intersectionCounter;
};


#endif