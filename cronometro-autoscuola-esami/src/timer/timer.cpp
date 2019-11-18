#include "timer.h"

#include <Arduino.h>
#include <BluetoothSerial.h>


Timer::Timer() = default;

void Timer::setup(bool isServer){
    if(_isSetup){
        Serial.println("[Timer] Already setup");
        return;
    };
    _isSetup = true;
    _isServer = isServer;
    pinMode(GPIO_NUM_37, INPUT);
    pinMode(GPIO_NUM_38, INPUT);
    pinMode(GPIO_NUM_39, INPUT);
    if(_isServer){
        Serial.println("[Timer] Starting as server...");
        _server.setup(_serverName, _clientName, _pingInterval);
    } else {
        Serial.println("[Timer] Starting as client...");
        _client.setup(_serverName, _clientName, _pingInterval);
    }
    Serial.println("[Timer] Setup done");
};

void Timer::loop(){
    _isServer ? _server.loop() : _client.loop();
};
