#include "timer.h"

#include <Arduino.h>


Timer::Timer() = default;

void Timer::setup(bool serverMode){
    if(isSetup){
        Serial.println("[Timer] Already setup");
        return;
    };
    isSetup = true;
    isServer = serverMode;
    if(isServer){
        Serial.println("[Timer] Starting as server...");
        server.setup();
    } else {
        Serial.println("[Timer] Starting as client...");
        client.setup();
    }
    Serial.println("[Timer] Setup done");
};

void Timer::loop(){

};
