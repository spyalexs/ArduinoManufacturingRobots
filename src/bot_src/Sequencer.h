#ifndef sequencer_h
#define sequencer_h

#include <Arduino.h>
#include <queue>

#include "Communicator.h"

#include "Command.h"
#include "FollowLineOnMarker.h"
#include "FollowLineUntilAction.h"
#include "FollowLineUntilMarker.h"
#include "TestCommand.h"
#include "TravelStraight.h"
#include "TurnLeft.h"
#include "TurnRight.h"

//the need for this arises from the fact that an abstract class cannot be store in a queue 
//this solved the problem by creating a place to store all of the descendant of that class

//give manage giving the next command - each command type must be regisitered in the sequecer
// 1 - as a queue of the type as a private member varibales
// 2 - in the command switch - use command name - typeof doesn't work in arduino 
// 3 - create a loader function

class Sequencer{
    public:
        Sequencer();

        void setCommunicatorPointer(Communicator* CC);

        void loadInCommand(FollowLineOnMarker command);
        void loadInCommand(FollowLineUntilAction command);
        void loadInCommand(FollowLineUntilMarker command);
        void loadInCommand(TestCommand command);
        void loadInCommand(TravelStraight command);
        void loadInCommand(TurnLeft command);
        void loadInCommand(TurnRight command);

        void clear();
        void removeCurrentCommand();

        Command* getNextCommand();
        Command* getCurrentCommand();

        bool isEmpty();
    
    private:
        std::queue<FollowLineOnMarker> m_followLineOnMarkerQueue;
        std::queue<FollowLineUntilAction> m_followLineUntilActionQueue;
        std::queue<FollowLineUntilMarker> m_followLineUntilMarkerQueue;
        std::queue<TestCommand> m_testCommandQueue;
        std::queue<TravelStraight> m_travelStraightQueue;
        std::queue<TurnLeft> m_turnLeftQueue;
        std::queue<TurnRight> m_turnRightQueue;

        std::queue<String> m_sequence;

        bool m_clearQueue = false;

        Communicator* mp_CC;

        void updateStatus(uint8_t status);
};




#endif