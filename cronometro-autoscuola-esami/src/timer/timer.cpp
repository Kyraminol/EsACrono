#include "timer.h"

#include <Arduino.h>
#include <BluetoothSerial.h>


Timer::Timer() = default;

void Timer::setup(){
    if(_isSetup){
        Serial.println("[Timer] Already setup");
        return;
    };
    _isSetup = true;

    pinMode(SERVER_SWITCH, INPUT);
    pinMode(TIMER_SWITCH, INPUT);
    pinMode(STOP_SWITCH, INPUT);
    pinMode(REMOTE_SWITCH, INPUT);
    pinMode(START_BUTTON, INPUT);
    pinMode(RESET_BUTTON, INPUT);
    pinMode(LEDMATRIX_SWITCH, INPUT);
    pinMode(LEDMATRIX_DATA, OUTPUT);
    pinMode(LEDMATRIX_BRIGHTNESS_BUTTON, INPUT);
    pinMode(RESISTOR_PASSWORD, INPUT);

    String password = "";
    int serverSwitch = digitalRead(SERVER_SWITCH);
    if(serverSwitch == LOW){
        Serial.println("[Timer] Starting as server...");
        _isServer = true;
        _server.setup(_serverName, _clientName, password, _pingInterval);
    } else {
        Serial.println("[Timer] Starting as client...");
        _client.setup(_serverName, _clientName, password, _pingInterval);
    }
    Serial.println("[Timer] Setup done");
};

void Timer::loop(){
    _isServer ? _server.loop() : _client.loop();
};
