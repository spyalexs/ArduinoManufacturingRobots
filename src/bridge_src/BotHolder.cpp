#include "BotHolder.h"
#include "arduino.h"

BotHolder::BotHolder(int maxBots){
    m_maxNumberOfBots = maxBots;
    m_botCount = 0;
}

int BotHolder::getBotCount(){
    return this->m_botCount;
}

bool BotHolder::appendNewBot(Bot bot){
    //adds a new bot to the bot holder
    //returns if the bot is successfully added

    if(this->m_botCount < this->m_maxNumberOfBots){
     //add bot to array

      //assign a new bot - this code is ugly
      switch(m_botCount){
        case 0:
          this->m_bot1 = bot;
          (mp_connections)->Connected(1, bot.getAddress());
          break;        
        case 1:
          this->m_bot2 = bot;
          (mp_connections + 1)->Connected(2, bot.getAddress());
          break;        
        case 2:
          this->m_bot3 = bot;
          (mp_connections + 2)->Connected(3, bot.getAddress());
          break;        
        case 3:
          this->m_bot4 = bot;
          (mp_connections + 3)->Connected(4, bot.getAddress());
          break;        
        case 4:
          this->m_bot5 = bot;
          (mp_connections + 4)->Connected(5, bot.getAddress());
          break;       
        case 5:
          this->m_bot6 = bot;
          (mp_connections + 5)->Connected(6, bot.getAddress());
          break;        
        case 6:
          this->m_bot7 = bot;
          (mp_connections + 6)->Connected(7, bot.getAddress());
          break;        
        case 7:
          this->m_bot8 = bot;
          (mp_connections + 7)->Connected(8, bot.getAddress());
          break;        
        case 8:
          this->m_bot9 = bot;
          (mp_connections + 8)->Connected(9, bot.getAddress());
          break;        
        case 9:
          this->m_bot10 = bot;
          (mp_connections + 9)->Connected(10, bot.getAddress());
          break;
      }

      this->m_botCount++;
      return true;
    } else {
      //full return fail
      return false;
    }
}

std::string BotHolder::getConnectionsString(){
    std::string connectionString = "Connected to: " + std::to_string(this->m_botCount) + " bots!";

    return connectionString;
}

void BotHolder::setConnectionsPtr(Connection* ptr){
  this->mp_connections = ptr;
}


void BotHolder::cycle(){
    //run the cycle for each of the bots

    BLE.poll();

    if(this->m_botCount >= 1){
        m_bot1.cycle((mp_connections)->m_isConnected);
    }
    
    if(this->m_botCount >= 2){
        m_bot2.cycle((mp_connections + 1)->m_isConnected);
    }

    if(this->m_botCount >= 3){
        m_bot3.cycle((mp_connections + 2)->m_isConnected);
    }

    if(this->m_botCount >= 4){
        m_bot4.cycle((mp_connections + 3)->m_isConnected);
    }
    
    if(this->m_botCount >= 5){
        m_bot5.cycle((mp_connections + 4)->m_isConnected);
    }
    
    if(this->m_botCount >= 6){
        m_bot6.cycle((mp_connections + 5)->m_isConnected);
    }
    
    if(this->m_botCount >= 7){
        m_bot7.cycle((mp_connections + 6)->m_isConnected);
    }
    
    if(this->m_botCount >= 8){
        m_bot8.cycle((mp_connections + 7)->m_isConnected);
    }
    
    if(this->m_botCount >= 9){
        m_bot9.cycle((mp_connections + 8)->m_isConnected);
    }
    
    if(this->m_botCount >= 10){
        m_bot10.cycle((mp_connections + 9)->m_isConnected);
    }
}

Bot* BotHolder::getBotByName(std::string name){
  //returns a pointer to the bot who has the matching name

    if(this->m_botCount >= 1){
      if(this->m_bot1.getName() == name){
        return &(this->m_bot1);
      }
    }

    if(this->m_botCount >= 2){
      if(this->m_bot2.getName() == name){
        return &(this->m_bot2);
      }
    }

    if(this->m_botCount >= 3){
      if(this->m_bot3.getName() == name){
        return &(this->m_bot3);
      }
    }

    if(this->m_botCount >= 4){
      if(this->m_bot4.getName() == name){
        return &(this->m_bot4);
      }
    }

        if(this->m_botCount >= 5){
      if(this->m_bot5.getName() == name){
        return &(this->m_bot5);
      }
    }

        if(this->m_botCount >= 6){
      if(this->m_bot6.getName() == name){
        return &(this->m_bot6);
      }
    }

    if(this->m_botCount >= 7){
      if(this->m_bot7.getName() == name){
        return &(this->m_bot7);
      }
    }

        if(this->m_botCount >= 8){
      if(this->m_bot8.getName() == name){
        return &(this->m_bot8);
      }
    }

        if(this->m_botCount >= 9){
      if(this->m_bot9.getName() == name){
        return &(this->m_bot9);
      }
    }

        if(this->m_botCount >= 10){
      if(this->m_bot10.getName() == name){
        return &(this->m_bot10);
      }
    }
}

void BotHolder::sendMessageToBot(MessageLine message){
  //send a message to the bot

  //get the target bot class
  Bot* target = this->getBotByName(message.m_target.c_str());

  //send message via class
  target->publishMessageToBot(message.m_characteristic, message.m_value.toInt());
}
