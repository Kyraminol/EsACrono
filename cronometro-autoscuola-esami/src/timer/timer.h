#ifndef TIMER_H
#define TIMER_H

#include <WiFi.h>
#include "BluetoothSerial.h"

class Timer{
public:
    Timer();
    void init(bool serverMode);
    void loop();
private:
    char *serverName = "Cronometro";
    char *clientName = "Client";

    bool isServer;
    bool isInit = false;

    unsigned int timers[2] = {0, 0};
    float results[2];

    BluetoothSerial SerialBT;
    WiFiClient webclient;
    WiFiServer webserver;
    const unsigned int maxBuffer = 100;
    char printBuff[100];

    void timerStart(short unsigned int timer);
    void timerStop(short unsigned int timer);

    void serverInit();
    void serverLoop();

    void clientInit();
    void clientLoop();
};


#endif
