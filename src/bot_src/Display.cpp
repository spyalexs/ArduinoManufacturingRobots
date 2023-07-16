#include "Display.h"

Display::Display(): m_tft(7, 10, 8, 9){
    this->m_tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_DIN, TFT_CLK); //change constructor to pass in more pin params  

    m_tft.begin();
    
    //display horizontal
    this->m_tft.setRotation(3);

    //display connection Icon
}

Display::~Display(){

}

void Display::setBackground(uint16_t color){
    //set screen background
    this->m_tft.fillScreen(color);
}

void Display::drawBasicUI(){
    //draws the basic UI before the loop begins

    this->m_tft.setTextColor(ILI9341_WHITE);
    
    //draw top bar
    this->m_tft.drawLine(0, DISPLAY_TOP_BAR_HEIGHT, DISPLAY_WIDTH - 1, DISPLAY_TOP_BAR_HEIGHT, ILI9341_WHITE);
    this->m_tft.setCursor(10, 5);
    this->m_tft.setTextSize(2);
    this->m_tft.print("Destination: ");

    //draw bottom bar

    //name
    this->m_tft.drawLine(0, DISPLAY_HEIGHT - DISPLAY_BOTTOM_BAR_HEIGHT, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - DISPLAY_BOTTOM_BAR_HEIGHT, ILI9341_WHITE);
    this->m_tft.setCursor(10, DISPLAY_HEIGHT - 14);
    this->m_tft.setTextSize(1);
    this->m_tft.print("Robot: ");    
    
    //Battery
    this->m_tft.setCursor(90, DISPLAY_HEIGHT - 14);
    this->m_tft.setTextSize(1);
    this->m_tft.print("Battery: ");    
    
    //connection
    this->m_tft.setCursor(220, DISPLAY_HEIGHT - 14);
    this->m_tft.setTextSize(1);
    this->m_tft.print("Connection: ");

    //connection icon
    this->m_connectionIcon.updateShape(DISPLAY_BOTTOM_BAR_HEIGHT -4, DISPLAY_BOTTOM_BAR_HEIGHT -4, 290, DISPLAY_HEIGHT - (DISPLAY_BOTTOM_BAR_HEIGHT - 2));
    
    //battery icon
    this->m_batteryIcon.updateShape(DISPLAY_BOTTOM_BAR_HEIGHT - 4, DISPLAY_BOTTOM_BAR_HEIGHT - 4, 150, DISPLAY_HEIGHT - (DISPLAY_BOTTOM_BAR_HEIGHT - 2));
}

bool Display::addPixelToBuffer(Pixel pixel){
    //check to see if there is room
    //if the pixels in buffer count is about to reach the write pixel counter,there is no room
    //  1. buffer is one less than write
    //  2. write is zero and buffer is max

    if(this->m_pixelsInBuffer + 1 == this->m_writePixel){
        //buffer is full
        return false;
    } else if(this->m_pixelsInBuffer == (PIXEL_BUFFER_LENGTH - 1) && this->m_writePixel == 0){
        //buffer is full
        return false;
    }

    //write to buffer
    this->m_pixelBuffer[this->m_pixelsInBuffer] = pixel;

    //increment pixels in buffer counter
    this->m_pixelsInBuffer++;
    if(this->m_pixelsInBuffer == PIXEL_BUFFER_LENGTH){
        this->m_pixelsInBuffer = 0;
    }

    return true;
}

void Display::cycle(){
    if(this->m_skipCycle == true){
        //skip cycle for the cause
        this->m_skipCycle = false;
        return;
    }

    int pixelsUpdated = 0;
    long cycleEnd = micros() + this->m_cycleMicros;

    //if buffer is empty - add job
    if(this->getPixelsInBufferCount() == 0 && this->m_jobQueue.empty() == false){
        //jobs may not be blocking for more than 10ms

        //wether the job completed
        bool pop = false;

        switch (this->m_jobQueue.front().m_jobNumber){
            //call job based on code from queue
            case 1:
                pop = this->drawRect(this->m_jobQueue.front().m_param1, this->m_jobQueue.front().m_param2, this->m_jobQueue.front().m_param3, this->m_jobQueue.front().m_param4,this->m_jobQueue.front().m_param5);
                break;
            case 2:
                pop = this->writeText();
                break;
            case 100:
                pop = this->drawIconOutline(&this->m_displayIcons[0]);
                break;
            case 101: 
                pop = this->drawIconOutline(&this->m_displayIcons[1]);
                break;           
            case 102:
                pop = this->drawIconOutline(&this->m_displayIcons[2]);
                break;
            case 103: 
                pop = this->drawIconOutline(&this->m_displayIcons[3]);
                break;           
            case 104:
                pop = this->drawIconOutline(&this->m_displayIcons[4]);
                break;
            case 105:  
                pop = this->drawIconOutline(&this->m_displayIcons[5]);
                break;          
            case 106:                
                pop = this->drawIconOutline(&this->m_displayIcons[6]);
                break;
            case 107:                
                pop = this->drawIconOutline(&this->m_displayIcons[7]);
                break;
            case 110:
                pop = this->drawIcon(&this->m_displayIcons[0]);
                break;
            case 111: 
                pop = this->drawIcon(&this->m_displayIcons[1]);
                break;           
            case 112:
                pop = this->drawIcon(&this->m_displayIcons[2]);
                break;
            case 113: 
                pop = this->drawIcon(&this->m_displayIcons[3]);
                break;           
            case 114:
                pop = this->drawIcon(&this->m_displayIcons[4]);
                break;
            case 115:  
                pop = this->drawIcon(&this->m_displayIcons[5]);
                break;          
            case 116:                
                pop = this->drawIcon(&this->m_displayIcons[6]);
                break;
            case 117:                
                pop = this->drawIcon(&this->m_displayIcons[7]);
                break;
            case 120:
                pop = this->drawIcon(&this->m_connectionIcon);
                break;            
            case 121:
                pop = this->drawIcon(&this->m_batteryIcon);
                break;
            case 255:
                pop = this->wipeDisplay(this->m_jobQueue.front().m_param1, this->m_jobQueue.front().m_param2);
                break;
        }

        if(pop){
            //remove job from queue
            this->m_jobQueue.pop();
        }
    }

    while(this->getPixelsInBufferCount() != 0){
        //check to end cycle based on clock
        if(pixelsUpdated > this->m_clockCheck){

            //check time
            if(micros() > cycleEnd){
                //the cycle is over 

                return;
            }
            pixelsUpdated = 0;
        }

        //there are pixels to be written -- write next pixel to screen
        this->m_tft.drawPixel(this->m_pixelBuffer[this->m_writePixel].m_x, this->m_pixelBuffer[this->m_writePixel].m_y, this->m_pixelBuffer[this->m_writePixel].m_color);

        //increment pixel
        this->m_writePixel ++;
        pixelsUpdated ++;

        //check for buffer loop
        if(this->m_writePixel == PIXEL_BUFFER_LENGTH){
            //has reached end of buffer - start over
            this->m_writePixel = 0;
        }
    }
}

bool Display::setIconsCount(int icons){

    if(icons != this->m_requestedIconCount){
        //if the number of icons is changing

        switch (icons){
            //if a doable value send to be drawn
            case 2: 
            case 4: 
            case 6:
            case 8:
                this->m_requestedIconCount = icons;
                drawIconSlots();
        
                break;

            default:
                Serial.println("Unable to adjust the number of display icons to :" + String(icons));
        }
    }
}

void Display::drawIconSlots(){
    //get heights and widths
    int iconZoneHeight = DISPLAY_HEIGHT - DISPLAY_BOTTOM_BAR_HEIGHT - DISPLAY_TOP_BAR_HEIGHT;
    int maxIconHeight = round((iconZoneHeight - DISPLAY_MIN_ICON_SPACING * 3) / 2);
    int maxIconWidth = round((DISPLAY_WIDTH - (int(this->m_requestedIconCount / 2) + 1) * DISPLAY_MIN_ICON_SPACING) / (this->m_requestedIconCount / 2));

    //the size of the icons - there a square
    int iconSize = 0;
    int xSpacing = DISPLAY_MIN_ICON_SPACING, ySpacing = DISPLAY_MIN_ICON_SPACING;
    if (maxIconWidth > maxIconHeight){
        iconSize = maxIconHeight;
        xSpacing = round((DISPLAY_WIDTH - (iconSize * this->m_requestedIconCount / 2)) / (this->m_requestedIconCount / 2 + 1));
    } else {
        iconSize = maxIconWidth;
        ySpacing = round((iconZoneHeight - (iconSize * 2)) / 3);
    }

    //go through each pair of vertically stacked icons and define them
    for(int i = 0; i < m_requestedIconCount; i += 2){
        //upper icon
        this->m_displayIcons[i].updateShape(iconSize, iconSize, round((i / 2) * (iconSize + xSpacing) + xSpacing), DISPLAY_TOP_BAR_HEIGHT + ySpacing);
        //lower icon
        this->m_displayIcons[i+1].updateShape(iconSize, iconSize, round((i / 2) * (iconSize + xSpacing) + xSpacing), DISPLAY_TOP_BAR_HEIGHT + ySpacing * 2 + iconSize);

        //draw the icons
        this->m_jobQueue.push(DisplayJob(100 + i));
        this->m_jobQueue.push(DisplayJob(101 + i));
    }
}

bool Display::drawIconOutline(Icon* icon){
    //draw the outline of the icon

    //add in  vertical lines
    for(int i = 0; i < icon->m_height; i++){
        //add left and right pixels
        this->addPixelToBuffer(Pixel(icon->m_x, icon->m_y + i, ILI9341_WHITE));
        this->addPixelToBuffer(Pixel(icon->m_x + icon->m_width, icon->m_y + i, ILI9341_WHITE));
    }    
    
    for(int i = 0; i < icon->m_width; i++){
        //add top and bottom pixels
        this->addPixelToBuffer(Pixel(icon->m_x + i, icon->m_y, ILI9341_WHITE));
        this->addPixelToBuffer(Pixel(icon->m_x + i, icon->m_y + icon->m_height, ILI9341_WHITE));
    }

    return true;
}

int Display::getPixelsInBufferCount(){
    //get the number of pixels that need to be written
    if(this->m_writePixel <= this->m_pixelsInBuffer){
        return this->m_pixelsInBuffer - this->m_writePixel;
    }

    return PIXEL_BUFFER_LENGTH - this->m_writePixel + this->m_pixelsInBuffer;
}

void Display::drawPacket(uint8_t* packetBuffer){

    //get the icon based on the front job
    if(this->m_jobQueue.front().m_jobNumber < 110 || this->m_jobQueue.front().m_jobNumber > 129){
        // the joib is not a draw icon
        Serial.println("Job is not a draw icon, returning");
        return;
    }

    //get the icon number from the job queue
    uint8_t iconNumber = this->m_jobQueue.front().m_jobNumber - 110;

    if(this->getPixelsInBufferCount() != 0){
        //display buffer should always be empty
        Serial.println("Attempting to draw packet on non empty display buffer!");

        return;
    }
    
    //uint8_t packet[1000] = {0, 0, 112, 2, 0, 32, 0, 32, 104, 2, 104, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 57, 231, 185, 231, 209, 234, 201, 233, 185, 231, 185, 231, 185, 231, 201, 233, 201, 233, 209, 234, 209, 234, 57, 231, 33, 228, 91, 235, 91, 235, 91, 235, 75, 233, 73, 229, 152, 35, 160, 68, 160, 68, 160, 68, 160, 68, 160, 68, 160, 68, 160, 68, 168, 166, 168, 166, 160, 68, 160, 68, 128, 35, 104, 229, 104, 229, 104, 229, 145, 230, 145, 230, 160, 68, 152, 35, 152, 35, 152, 35, 160, 68, 160, 68, 160, 68, 160, 68, 160, 68, 160, 68, 160, 68, 160, 68, 135, 240, 231, 252, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 80, 1, 0, 32, 33, 228, 24, 227, 152, 35, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 128, 35, 152, 35, 160, 68, 112, 2, 112, 2, 112, 2, 112, 2, 168, 166, 185, 231, 185, 231, 185, 231, 201, 233, 201, 233, 135, 240, 191, 247, 107, 237, 231, 252, 159, 243, 49, 230, 49, 230, 107, 237, 107, 237, 107, 237, 107, 237, 57, 231, 80, 1, 160, 68, 160, 68, 160, 68, 168, 166, 160, 68, 160, 68, 160, 68, 160, 68, 160, 68, 160, 68, 160, 68, 160, 68, 160, 68, 160, 68, 160, 68, 160, 68, 160, 68, 152, 35, 128, 35, 104, 229, 57, 231, 16, 130, 135, 240, 135, 240, 191, 247, 201, 233, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 80, 1, 33, 228, 16, 130, 57, 231, 168, 166, 104, 2, 104, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 128, 35, 112, 2, 112, 2, 16, 130, 24, 227, 112, 2, 112, 2, 160, 68, 168, 166, 185, 231, 185, 231, 201, 233, 209, 234, 91, 235, 135, 240, 75, 233, 135, 240, 159, 243, 159, 243, 191, 247, 159, 243, 75, 233, 0, 32, 49, 230, 107, 237, 107, 237, 91, 235, 91, 235, 91, 235, 24, 227, 0, 32, 0, 32, 0, 32, 0, 32, 0, 32, 0, 32, 0, 32, 0, 32, 0, 32, 0, 32, 0, 32, 0, 32, 16, 130, 0, 32, 24, 227, 107, 237, 135, 240, 135, 240, 159, 243, 191, 247, 191, 247, 209, 234, 0, 0, 0, 0, 0, 0, 0, 0, 56, 66, 33, 228, 49, 230, 0, 32, 80, 1, 104, 2, 104, 2, 104, 2, 104, 2, 104, 2, 104, 2, 104, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 0, 32, 0, 32, 16, 130, 24, 227, 112, 2, 112, 2, 152, 35, 160, 68, 168, 166, 185, 231, 201, 233, 209, 234, 80, 1, 191, 247, 75, 233, 135, 240, 107, 237, 91, 235, 57, 231, 57, 231, 107, 237, 135, 240, 135, 240, 75, 233, 24, 227, 16, 130, 16, 130, 24, 227, 49, 230, 49, 230, 49, 230, 49, 230, 49, 230, 33, 228, 24, 227, 16, 130, 16, 130, 75, 233, 107, 237, 16, 130, 16, 130, 0, 32, 0, 32, 0, 32, 0, 32, 107, 237, 75, 233, 191, 247, 155, 235, 209, 234, 209, 234, 0, 0, 0, 0, 0, 0, 80, 1, 75, 233, 57, 231, 49, 230, 24, 227, 160, 68, 88, 2, 88, 2, 104, 2, 104, 2, 104, 2, 104, 2, 104, 2, 104, 2, 104, 2, 104, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 112, 2, 33, 228, 0, 32, 57, 231, 49, 230, 24, 227, 16, 130, 235, 224, 185, 231, 128, 35, 112, 2, 168, 166, 185, 231, 201, 233, 209, 234, 219, 235, 201, 233, 24, 227, 159, 243, 57, 231, 159, 243, 135, 240, 159, 243, 91, 235, 57, 231, 231, 252, 0, 32, 49, 230, 16, 130, 24, 227, 24, 227, 24, 227, 24, 227, 16, 130, 16, 130, 16, 130, 16, 130, 16, 130, 24, 227, 75, 233, 152, 35, 107, 237, 16, 130, 0, 32, 0, 32, 0, 32, 16, 130, 91, 235, 57, 231, 159, 243, 209, 234, 209, 234, 209, 234, 185, 233, 0, 0, 0, 0, 80, 1, 0, 32, 49, 230, 33, 228, 24, 227, 88, 2, 88, 2, 88, 2, 88, 2, 80, 1, 88, 2, 88, 2, 104, 2, 104, 2, 104, 2, 104, 2, 104, 2, 104, 2, 104, 2, 112, 2, 112, 2, 112, 2, 112, 2, 16, 130, 57, 231, 24, 227, 24, 227, 16, 130, 0, 32, 16, 130, 235, 224, 235, 224, 128, 35, 128, 35, 160, 68, 185, 231, 201, 233, 209, 234, 209, 234, 209, 234, 209, 234, 185, 231, 185, 231, 16, 130, 107, 237, 49, 230, 16, 130, 16, 130, 49, 230, 75, 233, 75, 233, 57, 231, 16, 130, 16, 130, 16, 130, 16, 130, 16, 130, 16, 130, 16, 130, 16, 130, 33, 228, 159, 243, 159, 243, 107, 237, 16, 130, 16, 130, 0, 32, 0, 32, 24, 227, 135, 240, 135, 240, 209, 234, 209, 234, 209, 234, 185, 231, 57, 231, 0, 0};
    int startingx = this->getIcon(iconNumber)->m_x + 1;
    int startingy = this->getIcon(iconNumber)->m_y + getIcon(iconNumber)->getPacketRow() + 1;
    int size = this->getIcon(iconNumber)->getIconImageSize();
    int packetSize = this->getIcon(iconNumber)->getBytesPerPacket();

    for(int i = 0; i < packetSize; i += 2){
        uint16_t color = packetBuffer[i] << 8 | packetBuffer[i+1];

        this->addPixelToBuffer(Pixel(startingx + (int(i / 2) % size), startingy + floor((i / 2) / size), color));
    }

    //tell the display to skip the cycle as running this function counts
    //allows system to maintain overall cycle time
    this->m_skipCycle = true;
}

bool Display::drawIcon(Icon* icon){
    //draw an icon by requesting packets

    //check for end
    if(!icon->m_isDrawing){
        //icon is no longer drawing - job is over
        return true;
    }

    if(this->mp_messagesOutQueue == nullptr){
        //cannot draw icon as cannot find commuicator pointer
        Serial.println("Cannot draw icon as cannot find commuicator pointer!");
        return true;
    }

    //if a packet needs requested
    int nextPacket = icon->getNextPacketNumber();
    if(nextPacket != -1){
        //a packet does need requested
        //add to communicator queue

        mp_messagesOutQueue->push("iconRequest$" + icon->m_iconName + "_" + String(icon->getIconImageSize()) + "_" + String(nextPacket));
    }


    return false;
}

void Display::setMessagesOutQueue(std::queue<String>* queue){
    // set the pointer to the messages out queue
    this->mp_messagesOutQueue = queue;
}

void Display::addIconDrawJob(uint8_t iconNumber, String iconName){
    //call this to begin drawing an icon

    this->m_jobQueue.push(DisplayJob(110 + iconNumber));
    this->getIcon(iconNumber)->beginDrawing(iconName);
}

Icon* Display::getIcon(uint8_t iconNumber){
    if(iconNumber < 10){
        //return an item icon
        return &(this->m_displayIcons[iconNumber]);
    } else if(iconNumber == 10){
        return &(this->m_connectionIcon);
    } else if(iconNumber == 11){
        return &(this->m_batteryIcon);
    }

    Serial.println("Could not find matching icon");
    return nullptr;
}

bool Display::drawRect(int x, int y, int w, int h, uint16_t color){
    //draw horizontal lines
    for(int i = x; i < x + w; i++){
        this->addPixelToBuffer(Pixel(i, y, color));
        this->addPixelToBuffer(Pixel(i, y + h, color));
    } 

    //draw vertical lines
    for(int i = y; i < y + h; i++){
        this->addPixelToBuffer(Pixel(x, i, color));
        this->addPixelToBuffer(Pixel(x + w, i, color));
    } 

    return true;
}

void Display::drawOpeningMenu(){
    //draw opening menu
    this->m_jobQueue.push(DisplayJob(1, 60, (DISPLAY_BOTTOM_BAR_HEIGHT + DISPLAY_TOP_BAR_HEIGHT) / 2 - 35,  70, 70, ILI9341_WHITE));
    this->m_jobQueue.push(DisplayJob(1, 190, (DISPLAY_BOTTOM_BAR_HEIGHT + DISPLAY_TOP_BAR_HEIGHT) / 2 - 35,  70, 70, ILI9341_WHITE));
}

void Display::setMenu(bool isMenu){
    if(isMenu != this->m_inMenu){
        //if menu status has changed

        //update menu status
        this->m_inMenu = isMenu;

        if(isMenu == true){
            // if a menu needs to be drawn
            this->drawOpeningMenu();

        } else {
            //if a run time interface needs to be drawn
            this->setIconsCount(4);
        }
    }
}

void Display::addWipeDisplayJob(bool entireDisplay, uint16_t color = ILI9341_BLACK){
    this->m_jobQueue.push(DisplayJob(255, entireDisplay, color));
}

bool Display::wipeDisplay(bool entireDisplay = true, uint16_t color = ILI9341_BLACK){
    //wipe the display

    //prep starting point
    if(this->m_jobQueue.front().m_param3 == 65535){
        if(entireDisplay){
            this->m_jobQueue.front().m_param3 = 0;
        } else {
            this->m_jobQueue.front().m_param3 = DISPLAY_TOP_BAR_HEIGHT;
        }
    }

    if(entireDisplay == true){
        //draw over entire display
        
        for(uint16_t y = m_jobQueue.front().m_param3; y < DISPLAY_HEIGHT; y++){
            for(uint16_t x = 0; x < DISPLAY_WIDTH; x++){
                this->addPixelToBuffer(Pixel(x, y, color));
            }

            if(PIXEL_BUFFER_LENGTH - this->getPixelsInBufferCount() < DISPLAY_WIDTH){
                //not enough room for another row
                
                if(y < DISPLAY_HEIGHT - 1){
                    //more rows still need drawn

                    //save position
                    this->m_jobQueue.front().m_param3 = y;

                    return false;
                }
            }
        }

    } else {
        //draw over main portion of display

        for(uint16_t y = m_jobQueue.front().m_param3; y < DISPLAY_HEIGHT - DISPLAY_BOTTOM_BAR_HEIGHT; y++){
            for(uint16_t x = 0; x < DISPLAY_WIDTH; x++){
                this->addPixelToBuffer(Pixel(x, y, color));
            }

            if(PIXEL_BUFFER_LENGTH - this->getPixelsInBufferCount() < DISPLAY_WIDTH){
                //not enough room for another row
                
                if(y < DISPLAY_HEIGHT - DISPLAY_BOTTOM_BAR_HEIGHT - 1){
                    //more rows still need drawn

                    //save position
                    this->m_jobQueue.front().m_param3 = y;

                    return false;
                }
            }
        }
    }

    return true;
}

void Display::addWriteTextJob(uint16_t x, uint16_t y, uint16_t color, uint8_t textSize, String text){
    this->m_jobQueue.push(DisplayJob(2, x, y, color, textSize, 65535, text));
}


bool Display::writeText(){
    //write text as display job

    uint8_t maxCharacters = 0;

    //account for text size - only one or two are options for now
    if(this->m_jobQueue.front().m_param4 == 1){
        maxCharacters = DISPLAY_MAX_SIZE_1_CHARACTERS_PER_CYCLE;
    } else if(this->m_jobQueue.front().m_param4 == 2){
        maxCharacters = DISPLAY_MAX_SIZE_2_CHARACTERS_PER_CYCLE;
    } else {
        Serial.println("Invalid text size : " + String(this->m_jobQueue.front().m_param4));
        return true;
    }

    //only do on first loop
    if(this->m_jobQueue.front().m_param5 == 65535){
        //prep for writing
        this->m_tft.setCursor(this->m_jobQueue.front().m_param1, this->m_jobQueue.front().m_param2);
        this->m_tft.setTextColor(this->m_jobQueue.front().m_param3);
        this->m_tft.setTextSize(this->m_jobQueue.front().m_param4);

        this->m_jobQueue.front().m_param5 = 0;
    }
    

    if(this->m_jobQueue.front().m_text.length() <= maxCharacters){
        //print entire string as there is room to
        this->m_tft.print(this->m_jobQueue.front().m_text);
    } else{
        //print first part of string
        this->m_tft.print(this->m_jobQueue.front().m_text.substring(0, maxCharacters));

        //update string that still needs printed
        this->m_jobQueue.front().m_text = this->m_jobQueue.front().m_text.substring(maxCharacters);

        return false;
    }

    return true;
}


