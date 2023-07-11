#ifndef icon_h
#define icon_h

class Icon{
    public:
        int m_height = 0;
        int m_width = 0;

        // min x, min y
        int m_x = 0;
        int m_y = 0;

        Icon();

        void updateShape(int height, int width, int x, int y);

        int getIconImageSize();


};


#endif