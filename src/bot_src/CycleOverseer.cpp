#include "CycleOverseer.h"

CycleOverseer::CycleOverseer(int cyclesPerSecond){
    this->m_cycleDuration = 1 / double(cyclesPerSecond) * 1000000;
    this->m_item = 0;
    this->m_cycleStartTime = micros();
}

void CycleOverseer::clock(){
    //record time taken to complete item
    this->m_clockTimes[this->m_item] = micros() - this->m_cycleStartTime;

    for(int i = 0; i < this->m_item; i++){
        this->m_clockTimes[this->m_item] -= this->m_clockTimes[i];
    }

    if(this->m_item - 2 <= CYCLE_OVERSEER_NUMBER_ITEMS){
        //increment item
        this->m_item++;
    } else {
        //clock being miss called
        Serial.println("CycleOveerseer::clock is being called to oftern per cycle!");
    }
}

void CycleOverseer::endCycle(){
    long endTime = micros();

    //perform last clock
    this->m_clockTimes[this->m_item] = endTime - this->m_cycleStartTime;
    for(int i = 0; i < m_item; i++){
            this->m_clockTimes[this->m_item] -= this->m_clockTimes[i];
    }

    if(endTime > this->m_cycleDuration + this->m_cycleStartTime){
        //print overshoot message
        Serial.println("Cycle duration was exceeded! Target duration is: " + String(this->m_cycleDuration / 1000.0) + "ms. Actual duration was: " + String((endTime - this->m_cycleStartTime) / 1000.0) + "ms.");

        for(int i = 0; i < CYCLE_OVERSEER_NUMBER_ITEMS; i++){
            Serial.println("Time taken to cycle " + String(this->m_items[i]) + ": " + this->m_clockTimes[i]);
        }

        //reset time for next cycle
        this->m_cycleStartTime = endTime;
    }else{ 
        if(CYCLE_OVERSEER_DISPLAY_BREAKDOWN){
            for(int i = 0; i < CYCLE_OVERSEER_NUMBER_ITEMS; i++){
                Serial.println("Time taken to cycle " + String(this->m_items[i]) + ": " + this->m_clockTimes[i]);
            }
        }

        //provide adjustment for cycle
        delayMicroseconds(this->m_cycleDuration - (endTime - this->m_cycleStartTime));
            
        //reset for next cycle -- because of delay  increment is fixed
        this->m_cycleStartTime = this->m_cycleStartTime + this->m_cycleDuration;
    }

    this->m_item = 0;
}