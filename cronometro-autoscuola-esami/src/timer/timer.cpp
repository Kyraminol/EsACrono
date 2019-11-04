#include "timer.h"

#include <Arduino.h>
#include <BluetoothSerial.h>


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
        server.setup(serverName, clientName);
    } else {
        Serial.println("[Timer] Starting as client...");
        client.setup(serverName, clientName);
    }
    Serial.println("[Timer] Setup done");
};

void Timer::loop(){

};
