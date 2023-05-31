#ifndef travelstraight_h
#define travelstraight_h

#include "Command.h"
#include "MotionController.h"

class TravelStraight : public Command{
  public:
    TravelStraight(BLECharacteristic* StatusC, BLECharacteristic* IssueC, MotionController* MC, int counts);

    void startup();
    void cycle();
    void cleanup();
    bool ifEnd();

    int m_straightCounts;
    int m_targetCounts = 0;
};


#endif