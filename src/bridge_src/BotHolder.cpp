#include "BotHolder.h"

BotHolder::BotHolder(int maxBots){
    this->m_maxNumberOfBots = maxBots;
    this->m_bots = (Bot*)malloc(sizeof(Bot) * maxBots);
    this->m_botCount = 0;
}

int BotHolder::getBotCount(){
    return this->m_botCount;
}

bool BotHolder::appendNewBot(Bot bot){
    //adds a new bot to the bot holder
    //returns if the bot is successfully added

    if(this->m_botCount < this->m_maxNumberOfBots){
        //add bot to array

        this->m_bots[this->m_botCount] = bot;
        this->m_botCount++;
        return true;
    } else {
        //full return fail
        return false;
    }
}