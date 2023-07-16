#ifndef displayjob_h
#define displayjob_h

#include "Arduino.h"

class DisplayJob{
    //defining jobs the display has in its queue

    public:
        DisplayJob(uint8_t jobNumber, uint16_t param1 = -1, uint16_t param2 = -1, uint16_t param3 = -1, uint16_t param4 = -1, uint16_t param5 = -1, String text = "");

        uint8_t m_jobNumber = 0;

        uint16_t m_param1;
        uint16_t m_param2;
        uint16_t m_param3;
        uint16_t m_param4;
        uint16_t m_param5;

        String m_text;
};

#endif