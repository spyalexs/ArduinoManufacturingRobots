#ifndef travelstraight_h
#define travelstraight_h

#include "Command.h"
#include "RobotContainer.h"

class TravelStraight : public Command{
  public:
    TravelStraight(RobotContainer* MC, Communicator* CC, int counts);

    void startup();
    void cycle();
    void cleanup();
    bool ifEnd();

    int m_straightCounts;
    int m_targetCounts = 0;
};


#endif