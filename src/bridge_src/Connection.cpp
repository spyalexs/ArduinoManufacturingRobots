#include "Connection.h"
#include <Arduino.h>

Connection::Connection(){
    //default values until connection is opened

    m_number = 0;
    m_address = "";
    m_isConnected = false;
}

void Connection::Connected(int number, std::string address){
    this->m_number = number;
    this->m_address = address;
    this->m_isConnected = true;
}