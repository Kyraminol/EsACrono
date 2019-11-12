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
        int n;
        int s;
        String endpoint = "/api/v1/";
        
        void sendRequest(String path);
};

#endif
