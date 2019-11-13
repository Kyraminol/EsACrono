#ifndef TIMERCLIENT_H
#define TIMERCLIENT_H

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <WiFi.h>
#include <HTTPClient.h>


class TimerClient{
    public:
        TimerClient();
        void setup(String server_name, String client_name);
        void loop();
    private:
        bool isSetup = false;
        String clientName;
        String serverName;
        BluetoothSerial SerialBT;
        int n;
        int s;
        String endpoint = "/api/v1/";
        WiFiClient client;
        HTTPClient http;


        
        void sendRequest(String path);
};

#endif
