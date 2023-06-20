#ifndef BotHolder_h
#define BotHolder_h

#include "Bot.h"
#include "Connection.h"
#include <string>

class BotHolder{
    //outfitted for 
    private:
        int m_botCount;
        int m_maxNumberOfBots;

        //the possible bots
        Bot m_bot1;
        Bot m_bot2;
        Bot m_bot3;
        Bot m_bot4;
        Bot m_bot5;
        Bot m_bot6;
        Bot m_bot7;
        Bot m_bot8;
        Bot m_bot9;
        Bot m_bot10;

        //the connection ptr
        Connection* mp_connections;
    
    public:
        BotHolder(int maxBots);

        void cycle();

        bool appendNewBot(Bot bot);
        void setConnectionsPtr(Connection* ptr);

        int getBotCount();
        std::string getConnectionsString();

};

#endif