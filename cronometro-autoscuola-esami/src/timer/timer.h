#ifndef TIMER_H
#define TIMER_H

#include "server/server.h"
#include "client/client.h"


class Timer{
    public:
        Timer();
        void setup(bool isServer);
        void loop();

    private:
        TimerServer _server;
        TimerClient _client;
        bool _isServer = false;
        bool _isSetup = false;
        int _pingInterval = 10000;  // milliseconds

        String _serverName = "Cronometro";
        String _clientName = "T";
};

#endif
