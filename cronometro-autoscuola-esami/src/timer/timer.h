#ifndef TIMER_H
#define TIMER_H

#include "server/server.h"
#include "client/client.h"


#define SERVER_SWITCH GPIO_NUM_36
#define TIMER_SWITCH GPIO_NUM_37
#define STOP_SWITCH GPIO_NUM_38
#define REMOTE_SWITCH GPIO_NUM_39

#define START_BUTTON GPIO_NUM_32
#define RESET_BUTTON GPIO_NUM_33

#define LEDMATRIX_SWITCH GPIO_NUM_13
#define LEDMATRIX_DATA GPIO_NUM_17


class Timer{
    public:
        Timer();
        void setup();
        void loop();

    private:
        TimerServer _server;
        TimerClient _client;
        bool _isServer = false;
        bool _isSetup = false;
        int _pingInterval = 1800;  // milliseconds

        String _serverName = "Cronometro";
        String _clientName = "T";
};

#endif
