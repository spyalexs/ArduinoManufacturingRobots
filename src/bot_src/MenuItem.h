#ifndef menuitem_h
#define menuitem_h

#include "Arduino.h"

class MenuItem{
    public:
        MenuItem();
        void redefine(uint8_t value, String text, uint16_t m_x, uint16_t m_y, uint16_t m_height, uint16_t m_width);

        uint8_t m_value;
        String m_text;

        //bounding box of menu item
        uint16_t m_x; 
        uint16_t m_y;
        uint16_t m_height;
        uint16_t m_width;
};

#endif