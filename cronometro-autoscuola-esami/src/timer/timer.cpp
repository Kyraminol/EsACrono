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
    pinMode(GPIO_NUM_37, INPUT);
    pinMode(GPIO_NUM_38, INPUT);
    pinMode(GPIO_NUM_39, INPUT);
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
    isServer ? server.loop() : client.loop();
};
