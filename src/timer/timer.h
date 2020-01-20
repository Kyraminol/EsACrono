#ifndef TIMER_H
#define TIMER_H

#include "server/server.h"
#include "client/client.h"

#include <ESPAsyncWebServer.h>

#define SERVER_SWITCH GPIO_NUM_36
#define TIMER_SWITCH GPIO_NUM_37
#define STOP_SWITCH GPIO_NUM_38
#define REMOTE_SWITCH GPIO_NUM_39

#define SERVER_T0_START GPIO_NUM_37
#define SERVER_T1_START GPIO_NUM_38

#define START_BUTTON GPIO_NUM_32
#define RESET_BUTTON GPIO_NUM_33

#define LEDMATRIX_SWITCH GPIO_NUM_23
#define LEDMATRIX_DATA GPIO_NUM_17
#define LEDMATRIX_BRIGHTNESS_BUTTON GPIO_NUM_2

#define CARDKB_SCL GPIO_NUM_22
#define CARDKB_SDA GPIO_NUM_23

#define RESISTOR_PASSWORD GPIO_NUM_13
#define RESISTOR_REFERENCE 1000

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
        int _pingInterval = 3000;  // milliseconds

        String _serverName = "Cronometro";
        String _clientName = "T";
};

void parseMsg(LinkedList<RequestParameter *>& paramsList, const String& params);
bool hasParam(const LinkedList<RequestParameter *>& params, const String& name);
RequestParameter* getParam(const LinkedList<RequestParameter *>& params, const String& name);

#endif
