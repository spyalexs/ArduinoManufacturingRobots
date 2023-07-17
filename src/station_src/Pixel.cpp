#include "Pixel.h"

Pixel::Pixel(){
    this->m_x = 0;
    this->m_y = 0;
    this->m_color = 0x0000;
}

Pixel::Pixel(uint16_t x, uint16_t y, uint16_t color){
    this->m_x = x;
    this->m_y = y;
    this->m_color = color;
}