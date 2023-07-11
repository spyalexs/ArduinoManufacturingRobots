#ifndef pixel_h
#define pixel_h

#include "arduino.h"

class Pixel{
    //class to store information about a pixel on the display

    public:
        Pixel();
        Pixel(uint16_t x, uint16_t y, uint16_t color);

        uint16_t m_x;
        uint16_t m_y;
        uint16_t m_color;
};

#endif