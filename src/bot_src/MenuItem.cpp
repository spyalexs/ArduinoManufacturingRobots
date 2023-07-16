#include "MenuItem.h"

MenuItem::MenuItem(){
    //dummy constructor
    this->m_x = -1;
    this->m_y = -1;
    this->m_height = -1;
    this->m_width = -1;
    this->m_value = -1;
    this->m_text = "";
}

void MenuItem::redefine(uint8_t value, String text, uint16_t x, uint16_t y, uint16_t width, uint16_t height){
    //change the menu item

    this->m_value = value;
    this->m_text = text;
    this->m_x = x;
    this->m_y = y;
    this->m_height = height;
    this->m_width = width;
}