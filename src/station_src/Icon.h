#ifndef icon_h
#define icon_h

#include "Arduino.h"

class Icon{
    public:
        int m_height = 0;
        int m_width = 0;

        // min x, min y
        int m_x = 0;
        int m_y = 0;

        int m_numberOfPackets = -1;
        int m_currentPacket = -1;

        bool m_isDrawing = false;
        bool m_hasRequestedPacket = false;

        String m_iconName = "";

        Icon();

        void updateShape(int height, int width, int x, int y);

        int getIconImageSize();

        void beginDrawing(String m_iconName);
        int getPacketRow();
        int getNextPacketNumber();
        int getBytesPerPacket();

    private:
        int strategicPacketNumber(int packetNumber);


};


#endif