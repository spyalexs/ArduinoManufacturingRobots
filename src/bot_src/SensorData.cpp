#include "SensorData.h"

SensorData::SensorData(uint16_t UV1, uint16_t UV2, uint16_t UV3, uint16_t US){
    this->m_UV1 = UV1;
    this->m_UV2 = UV2;
    this->m_UV3 = UV3;
    this->m_US = US;

    return;
}