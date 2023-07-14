#ifndef cycleoverseer_h
#define cycleoverseer_h

#define CYCLE_OVERSEER_DISPLAY_BREAKDOWN false
#define CYCLE_OVERSEER_NUMBER_ITEMS 5

#include <Arduino.h>

//class to ensure that the main code loop follows a cycle of a set time
class CycleOverseer{
    private:
        //seconds
        double m_clockTimes[CYCLE_OVERSEER_NUMBER_ITEMS] = {0,0,0,0,0};
        const char* m_items[CYCLE_OVERSEER_NUMBER_ITEMS] = {"Command", "Out Com", "In Com", "Com", "Container"};

        //microseconds
        double m_cycleDuration = .02;
        double m_cycleStartTime = 0;

        uint8_t m_item = 0;
    public:
        CycleOverseer(int cyclesPerSecond);

        void clock();
        void endCycle();
};

#endif