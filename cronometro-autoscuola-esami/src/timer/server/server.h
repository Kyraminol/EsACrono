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
        void timerSet(int timer, bool stop);
        String serverName;
        String clientName;
        BluetoothSerial SerialBT;
        AsyncWebServer webserver;
        void receiveLoRa();
        void clientRegister(int timer, bool stop);
};

#endif
