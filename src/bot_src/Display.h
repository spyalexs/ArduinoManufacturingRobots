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
#include "DisplayJob.h"
#include "MenuItem.h"

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

#define DISPLAY_MAX_SIZE_2_CHARACTERS_PER_CYCLE 7
#define DISPLAY_MAX_SIZE_1_CHARACTERS_PER_CYCLE 10

#define DISPLAY_MAX_MENU_ITEMS 10
#define DISPLAY_TEST_MENU_PAGES 5

#define PIXEL_BUFFER_LENGTH 1000

// backlight will eventually go in GPIO breakout board


class Display{
    private:
        //hardware SPI 
        Adafruit_ILI9341 m_tft;

        int m_cycleMicros = 10000; // how long to cycle the display for
        int m_clockCheck = 40; // after how many pixels to check the clock
        bool m_skipCycle = false; // if to skip the next cycle or not

        int m_writePixel = 0; //the next pixel to be written
        int m_pixelsInBuffer = 0; //the next pixel to be added to the queue
        Pixel m_pixelBuffer[PIXEL_BUFFER_LENGTH]; //array of all pixels that need written

        std::queue<DisplayJob> m_jobQueue; // queue of display jobs
        std::queue<String>* mp_messagesOutQueue = nullptr; // queue in comminucator of messages that need to be sent

        int m_requestedIconCount = 0; // the number of icons on the display

        Icon m_displayIcons[8];
        Icon m_connectionIcon; // the icon to display the connection
        Icon m_batteryIcon; // the icon to display the battery

        uint8_t m_selectedMenuItem = 0;
        uint8_t m_activeMenuItems = 0;
        MenuItem m_menuItems[DISPLAY_MAX_MENU_ITEMS];

        uint8_t m_testingMenuPage = 1; //if the display is in menu
        String m_testingPages[DISPLAY_TEST_MENU_PAGES] = {"Exit", "Motors", "LEDs", "Sensors1", "Sensors2"};

        //wipe display -- reoccuring function
        bool wipeDisplay(bool entireDisplay, uint16_t color);

        //write text -- reoccuring function
        bool writeText();

        //highlight menu item
        bool highlightMenuItem(uint8_t item, bool unhighlight = false);

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
        void cycle(int encoderCount);

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

        //draw opening menu
        void drawOpeningMenu();

        //draw rectangle
        bool drawRect(int x, int y, int w, int h, uint16_t color);

        //add wipeDisplayJob
        void addWipeDisplayJob(bool entireDisplay, uint16_t color);

        //add write text job
        void addWriteTextJob(uint16_t x, uint16_t y, uint16_t color, uint8_t textSize, String text);

        //draw a menu item
        void drawMenuItem(uint8_t item);

        //return the selected menu item
        uint8_t getSelectedMenuItem();

        //return a pointer to a menu item
        MenuItem* getMenuItemPointer(uint8_t item);

        //Draw the template for the testing menu
        void drawTestingMenu(uint8_t page = 1);

        //disable menus for running by setting the active menu items to zero
        void disableMenu();

        // returns the next testing menu page
        uint8_t getNextTestingMenuPage();

        //returns the previous testing menu page
        uint8_t getPreviousTestingMenuPage();

};

/*Job Reference
    001 - draw a rectangle
    002 - print text
    10x - draw icon outline number xx
    11x - draw icon image number xx
    255 - wipe display

*/
#endif
