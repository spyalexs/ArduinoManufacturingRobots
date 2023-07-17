#include "DisplayJob.h"

DisplayJob::DisplayJob(uint8_t jobNumber, uint16_t param1, uint16_t param2, uint16_t param3, uint16_t param4, uint16_t param5, String text){
    //set up a display job
    this->m_jobNumber = jobNumber;

    this->m_param1 = param1;
    this->m_param2 = param2;
    this->m_param3 = param3;
    this->m_param4 = param4;
    this->m_param5 = param5;

    this->m_text = text;
}