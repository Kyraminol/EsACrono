#ifndef TIMERCLIENT_H
#define TIMERCLIENT_H

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <FastLED_NeoMatrix.h>
#include <AsyncUDP.h>


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
        AsyncUDP _udp;
        
        void sendRequest(String path);
        void sendLoRa(String msg);

        int _lastPing = 0;
        void sendPing();

        static const int _matrixWidth = 5;
        static const int _matrixHeight = 5;
        static const int _matrixSize = _matrixWidth * _matrixHeight;
        CRGB _matrixLeds[_matrixSize];
        FastLED_NeoMatrix _matrix;
        int _lastMatrixRefresh = 0;
        void matrixRefresh();

        void sendMsg(String msg);
        String getClientType();

        void sendUDP(String msg);
};

#endif
