#ifndef followline_h
#define followline_h

#include "Command.h"
#include "MotionController.h"

class FollowLineUntilMarker : public Command{
  public:
    FollowLineUntilMarker(BLECharacteristic* StatusC, BLECharacteristic* IssueC, MotionController* MC);

    void startup();
    void cycle();
    void cleanup();
    bool ifEnd();

    MotionController* mp_MC;
};


#endif