#include "FollowLine.h"

FollowLine::FollowLine(BLECharacteristic* StatusC, BLECharacteristic* IssueC, int Time){
  m_time = Time;

  Command::Command(StatusC, IssueC);
}