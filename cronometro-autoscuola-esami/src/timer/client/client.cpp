#include "client.h"

#include <Arduino.h>
#include <BluetoothSerial.h>


TimerClient::TimerClient() = default;

void TimerClient::setup(String server, String client){
    if(isSetup){
        Serial.println("[Timer] Already setup");
        return;
    };
    isSetup = true;
    serverName = server;
    clientName = client;
    SerialBT.begin(clientName.c_str());
};

void TimerClient::loop(){

};
