#include "timer.h"

#include <WiFi.h>

Timer::Timer() = default;

void Timer::init(bool serverMode){
    if(isInit){
        return;
    }
    isInit = true;
    isServer = serverMode;
    Serial.begin(115200);
    if(isServer){
        serverInit();
    } else {
        clientInit();
    };

};

void Timer::loop(){
    if(SerialBT.available()){
        char c = SerialBT.read();
        switch(c){
            case '0':
                timerStart(0);
                break;
            case '1':
                timerStop(0);
                break;
            case '2':
                timerStart(1);
                break;
            case '3':
                timerStop(1);
                break;
            case '4':
                sprintf(printBuff, "%.1f", results[0]);
                for(int i; i < 100; i++){
                    if(printBuff[i] == '\0'){
                        break;
                    };
                    SerialBT.write(printBuff[i]);
                };
                SerialBT.write('\n');
                break;
        };
    };

    if(isServer){
        serverLoop();
    } else {
        clientLoop();
    };
};

void Timer::timerStart(short unsigned int timer){
    if(timers[timer] == 0){
        timers[timer] = millis();
    };
};

void Timer::timerStop(short unsigned int timer){
    if(timers[timer] > 0){
        results[timer] = (float)(millis() - timers[timer]) / 1000;
        timers[timer] = 0;
    };
};

void Timer::serverInit(){
    SerialBT.begin(serverName);
    WiFi.softAP(serverName);
    webserver = WiFiServer(80);
    webserver.begin();
};

void Timer::clientInit(){
    SerialBT.begin(clientName);
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.setAutoConnect(true);
    WiFi.begin(serverName);
    for(int i = 0; i < 10 && WiFi.status() == WL_DISCONNECTED; i++){
        delay(500);
    }
};

void Timer::serverLoop(){

};

void Timer::clientLoop(){

};
