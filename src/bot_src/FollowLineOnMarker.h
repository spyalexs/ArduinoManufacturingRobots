#ifndef followlineonmarker_h
#define followlineonmarker_h

#include "Command.h"
#include "MotionController.h"

class FollowLineOnMarker : public Command{
  public:
    FollowLineOnMarker(BLECharacteristic* StatusC, BLECharacteristic* IssueC, MotionController* MC);

    void startup();
    void cycle();
    void cleanup();
    bool ifEnd();

};


#endif