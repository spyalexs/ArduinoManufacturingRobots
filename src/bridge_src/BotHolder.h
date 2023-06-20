#ifndef BotHolder_h
#define BotHolder_h

#include "Bot.h"

class BotHolder{
    //outfitted for 
    private:
        Bot* m_bots;
        int m_botCount;
        int m_maxNumberOfBots;
    
    public:
        BotHolder(int maxBots);
        bool appendNewBot(Bot bot);
        int getBotCount();

};

#endif