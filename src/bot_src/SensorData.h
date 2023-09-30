#ifndef SENSORDATA_HH
#define SENSORDATA_HH

#include <arduino.h>

class SensorData{
    //class to hold the current sensor data;

    public:
        SensorData(uint16_t UV1, uint16_t UV2, uint16_t UV3, uint16_t US);

        uint16_t m_UV1;
        uint16_t m_UV2;
        uint16_t m_UV3;
        uint16_t m_US;
};


#endif