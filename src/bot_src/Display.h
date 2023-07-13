#ifndef display_h
#define display_h

#include <arduino.h>
#include <queue>

//class to control spi tft display via adafruit ili_9341
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#include "Pixel.h"
#include "Icon.h"

// For the Adafruit shield, these are the default.
#define TFT_DC 10
#define TFT_CS 7
#define TFT_DIN 8
#define TFT_CLK 9

#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240

#define DISPLAY_TOP_BAR_HEIGHT 26
#define DISPLAY_BOTTOM_BAR_HEIGHT 20

#define DISPLAY_MIN_ICON_SPACING 12

#define PIXEL_BUFFER_LENGTH 1000

// backlight will eventually go in GPIO breakout board


class Display{
    private:
        //hardware SPI 
        Adafruit_ILI9341 m_tft;

        int m_cycleMicros = 10000; // how long to cycle the display for
        int m_clockCheck = 40; // after how many pixels to check the clock

        int m_writePixel = 0; //the next pixel to be written
        int m_pixelsInBuffer = 0; //the next pixel to be added to the queue
        Pixel m_pixelBuffer[PIXEL_BUFFER_LENGTH]; //array of all pixels that need written

        std::queue<uint8_t> m_jobQueue; // queue of display jobs
        std::queue<String>* mp_messagesOutQueue = nullptr; // queue in comminucator of messages that need to be sent

        int m_requestedIconCount = 0; // the number of icons on the display

        Icon m_displayIcons[8];
        Icon m_connectionIcon; // the icon to display the connection
        Icon m_batteryIcon; // the icon to display the battery

    public:
        //constructor
        Display();

        ~Display();

        //set the number of icons on the display 
        bool setIconsCount(int icons);

        //set display background - do before the loop
        void setBackground(uint16_t color);

        //set up the basic UI - do before the loop
        void drawBasicUI();

        //draw icon slots - in loop
        void drawIconSlots();

        //cycle for a specified amount of time
        void cycle();

        //add pixel to pixel buffer so it can be written to screen
        bool addPixelToBuffer(Pixel pixel);

        //draw the outline of an icon
        bool drawIconOutline(Icon* icon);

        //returns how many pixels are in the buffer
        int getPixelsInBufferCount();

        //draw an individual packet recieve from central
        void drawPacket(uint8_t* packetBuffer);

        //draw an icon by requesting packets from central
        bool drawIcon(Icon* icon);

        //set the output queue
        void setMessagesOutQueue(std::queue<String>* queue);

        // call to actually start drawing an icon
        void addIconDrawJob(uint8_t iconNumber, String iconName);

        //return an icon
        Icon* getIcon(uint8_t iconNumber);
};

/*Job Reference
    10x - draw icon outline number xx
    11x - draw icon image number xx
*/
#endif
