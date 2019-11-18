#ifndef TIMERCLIENT_H
#define TIMERCLIENT_H

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <WiFi.h>
#include <HTTPClient.h>


class TimerClient{
    public:
        TimerClient();
        void setup(String serverName, String clientName, int pingInterval);
        void loop();
    private:
        bool _isSetup = false;
        String _clientName = "";
        String _serverName = "";
        int _pingInterval = 0;
        BluetoothSerial _SerialBT;
        int _t = 0;
        int _s = 0;
        String _endpoint = "/api/v1/";
        WiFiClient _client;
        HTTPClient _http;
        
        void sendRequest(String path);
        void sendLoRa(String msg);

        int _lastPing = 0;
        void sendPing();
};

#endif
