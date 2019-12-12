#ifndef TIMERSERVER_H
#define TIMERSERVER_H

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <ESPAsyncWebServer.h>

#include <FastLED_NeoMatrix.h>


class TimerServer{
    public:
        TimerServer();
        void setup(String serverName, String clientName, int pingInterval);
        void loop();
    private:
        bool _isSetup = false;
        int _timers[2] = {0, 0};
        float _results[2];
        void timerSet(int timer, bool stop);
        void timerReset(int timer=-1);
        String _serverName = "";
        String _clientName = "";
        int _pingInterval = 0;
        BluetoothSerial _SerialBT;
        AsyncWebServer _webserver;
        void receiveLoRa();
        void clientPinged(int timer, bool stop);
        int _pings[4] = {0, 0, 0, 0};
        int _lastPingCheck = 0;
        void pingCheck();

        int _matrixRed;
        int _matrixGreen;
        int _matrixBlue;
        static const int _matrixWidth = 32;
        static const int _matrixHeight = 16;
        static const int _matrixTileWidth = 1;
        static const int _matrixTileHeight = 2;
        static const int _matrixSize = _matrixWidth * _matrixHeight;
        CRGB _matrixLeds[_matrixSize];
        FastLED_NeoMatrix _matrix;

        static const int _matrixRefreshInterval = 10;
        int _lastMatrixRefresh = 0;
        void matrixRefresh();
        int _matrixBrightnessState = 2;
        int _matrixBrightness[5] = {10, 20, 50, 100, 255};
        int _lastMatrixBrightnessCicle = 0;
        void matrixBrightnessCicle();
};

#endif
