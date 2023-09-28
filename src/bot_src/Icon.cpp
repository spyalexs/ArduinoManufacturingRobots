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

    this->m_isDrawing = false;
    this->m_hasRequestedPacket = false;

    //get total number of packets for this shape
    //ciel(number of rows / floor(size of packet / (pixels per row * 2)))
    this->m_numberOfPackets = ceil((this->m_height - 1) / floor(900 / ((this->m_width - 1) * 2)));
}

int Icon::getIconImageSize(){
    //return the size of the required icon image for the display
    return this->m_height - 1;
}

void Icon::beginDrawing(String iconName){
    //ensure shape has been set before beginning drawing
    if(this->m_height == -1){
        Serial.println("Cannot begin drawing as shape has not been set");
        return;
    }

    this->m_isDrawing = true;
    this->m_hasRequestedPacket = false;
    this->m_currentPacket = 0;
    this->m_iconName = iconName;
}

int Icon::getPacketRow(){

    Serial.println("Drawing Packet");
    //get row - rows per packet * packet number
    int row = floor(900 / (this->m_width * 2)) * this->strategicPacketNumber(this->m_currentPacket) - 1;
    
    //increment the current packet
    this->m_currentPacket++;

    //if the current now equals max, the drawing is complete
    if(this->m_currentPacket == this->m_numberOfPackets){
        this->m_isDrawing = false;
    } else{
        this->m_hasRequestedPacket = false;
    }

    return row;
}

int Icon::getNextPacketNumber(){
    if(this->m_hasRequestedPacket == true){
        //if the next packet has already been requested return -1
        return -1;
    } 

    //marks the packet has requested
    this->m_hasRequestedPacket = true;
    //return the packet that needs requested
    return this->strategicPacketNumber(this->m_currentPacket);
}

int Icon::strategicPacketNumber(int packetNumber){
    //turns a normal packet number into a smarter one so the icon draws from the center first

    if(packetNumber % 2 == 0){
        //higher packets
        return floor((this->m_numberOfPackets) / 2) + ceil(packetNumber / 2.0);
    }

    //lower packets
    return floor((this->m_numberOfPackets) / 2) - ceil(packetNumber / 2.0);

}

int Icon::getBytesPerPacket(){
    //return the number of bytes in the current packet
    
    if(this->m_currentPacket == this->m_numberOfPackets - 1){
        // the last packet - this has the possibility of containing less rows
        // rows remaining * bytes per row
       
        return (this->getIconImageSize() - (this->m_currentPacket * floor(900 / ((this->m_width - 1) * 2)))) * (this->getIconImageSize() * 2);
    }


    //not the last packet so the packet will be as full as it can be
   return floor(900 / ((this->m_width - 1) * 2)) * (this->m_width - 1) * 2;
}


