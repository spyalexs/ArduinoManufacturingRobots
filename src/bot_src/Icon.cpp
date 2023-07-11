#include "Icon.h"

Icon::Icon(){

    int m_height = -1;
    int m_width = -1;

    // min x, min y
    int m_x = -1;
    int m_y = -1;
}

void Icon::updateShape(int height, int width, int x, int y){
    //update the shape of the Icon - place & size
    this->m_height = height;
    this->m_width = width;

    this->m_x = x;
    this->m_y = y;
}

int Icon::getIconImageSize(){
    //return the size of the required icon image for the display
    return this->m_height - 1;
}

