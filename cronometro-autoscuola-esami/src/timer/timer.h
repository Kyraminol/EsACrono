#ifndef TIMER_H
#define TIMER_H

#include <BluetoothSerial.h>

#include "server/server.h"
#include "client/client.h"


class Timer{
    public:
        Timer();
        void setup(bool serverMode);
        void loop();

    private:
        TimerServer server;
        TimerClient client;
        bool isServer = false;
        bool isSetup = false;

        String serverName = "Cronometro";
        String clientName = "";
};

#endif
