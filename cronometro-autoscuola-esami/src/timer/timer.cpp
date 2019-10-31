#include "timer.h"

#include <Arduino.h>


Timer::Timer() = default;

void Timer::setup(bool serverMode){
    if(isSetup){
        Serial.println("Timer is already setup");
        return;
    };
    isSetup = true;
    Serial.println("Setup done");

};

void Timer::loop(){

};
