#include "MessageLine.h"

MessageLine::MessageLine(String message){
    //parse raw message to a target characterisitc and value
    int firstSeperator = message.indexOf("$");

    //the target of the message
    m_target = message.substring(0, firstSeperator);
    String backString = message.substring(firstSeperator + 1);
    int secondSeperator = backString.indexOf("$");

    m_characteristic = backString.substring(0, secondSeperator);
    m_value = backString.substring(secondSeperator + 1);
}

MessageLine::MessageLine(String target, String characteristic, String value){
    m_target = target;
    m_characteristic = characteristic;
    m_value = value;
}

String MessageLine::getSerialString(){
    return "$$$$$" + m_target + "$" + m_characteristic + "$" + m_value;
}