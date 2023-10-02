#ifndef followlineonmarker_h
#define followlineonmarker_h

#include "Command.h"
#include "RobotContainer.h"

class FollowLineOnMarker : public Command{
  public:
    FollowLineOnMarker(RobotContainer* MC, Communicator* CC, bool preconfirmed = false);

    void startup();
    void cycle();
    void cleanup();
    bool ifEnd();

};


#endif