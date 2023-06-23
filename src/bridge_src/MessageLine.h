#ifndef messageline_h
#define messageline_h

#include <Arduino.h>

class MessageLine{
  //class to hold data gotten from the serial port

  public:
    MessageLine(String message);
    MessageLine(String target, String characteristic, String value);

    String getSerialString();

    String m_target;
    String m_characteristic;
    String m_value;
};

#endif
