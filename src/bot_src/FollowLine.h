#ifndef followline_h
#define followline_h

#include "Command.h"

class FollowLine : Command{
  FollowLine(BLECharacteristic* StatusC, BLECharacteristic* IssueC, int Time);

  int m_time;
};


#endif