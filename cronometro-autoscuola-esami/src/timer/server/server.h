#ifndef TIMERSERVER_H
#define TIMERSERVER_H

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <ESPAsyncWebServer.h>


class TimerServer{
    public:
        TimerServer();
        void setup(String server, String client);;
        void loop();
    private:
        bool isSetup = false;
        int timers[2] = {0, 0};
        float results[2];
        void timerStart(int timer);
        void timerStop(int timer);
        String serverName;
        String clientName;
        BluetoothSerial SerialBT;
        AsyncWebServer webserver;
        void receiveLoRa();
};

#endif
