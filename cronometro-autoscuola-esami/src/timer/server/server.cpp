#include "server.h"

#include <Arduino.h>
#include <WiFi.h>

TimerServer::TimerServer() = default;

void TimerServer::setup(){
    
};

void TimerServer::loop(){

};

void TimerServer::timerStart(int timer){
    if(timers[timer] == 0){
        timers[timer] = millis();
    };
};

void TimerServer::timerStop(int timer){
    if(timers[timer] > 0){
        results[timer] = (float)(millis() - timers[timer]) / 1000.0;
        timers[timer] = 0;
        Serial.println(results[timer]);
    };
};
