#ifndef TIMERSERVER_H
#define TIMERSERVER_H

#include <WiFi.h>

class TimerServer
{
public:
    TimerServer();

private:
    WiFiServer webserver;
};

#endif
