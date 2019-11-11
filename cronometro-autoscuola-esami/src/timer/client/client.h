#ifndef TIMERCLIENT_H
#define TIMERCLIENT_H

#include <Arduino.h>
#include <BluetoothSerial.h>


class TimerClient{
    public:
        TimerClient();
        void setup(String server, String client);
        void loop();
    private:
        bool isSetup = false;
        String clientName;
        String serverName;
        BluetoothSerial SerialBT;
        
        void sendRequest(String path);
};

#endif
