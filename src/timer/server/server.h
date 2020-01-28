#ifndef TIMERSERVER_H
#define TIMERSERVER_H

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <AsyncUDP.h>
#include <WebServer.h>

#include <FastLED_NeoMatrix.h>


class RequestParameter{
    private:
        String _name;
        String _value;

    public:
        RequestParameter(const String& name, const String& value): _name(name), _value(value){}
        const String& name() const { return _name; }
        const String& value() const { return _value; }
};


class TimerServer{
    public:
        TimerServer();
        void setup(String serverName, String clientName, String password, int pingInterval);
        void loop();
    private:
        bool _isSetup = false;
        int _timers[2] = {0, 0};
        float _results[2];
        int _stopped[2] = {0, 0};
        void timerSet(int timer, bool stop, bool ignoreMinInterval=false);
        void timerReset(int timer=-1);
        String _serverName = "";
        String _clientName = "";
        String _password = "";
        int _pingInterval = 0;
        BluetoothSerial _SerialBT;
        WebServer _webserver;
        AsyncUDP _udp;
        void clientPinged(int timer, bool stop, bool waiting);
        int _pings[4] = {0, 0, 0, 0};
        int _lastPingCheck = 0;
        void pingCheck();

        static const int _matrixWidth = 32;
        static const int _matrixHeight = 16;
        static const int _matrixTileWidth = 1;
        static const int _matrixTileHeight = 2;
        static const int _matrixSize = _matrixWidth * _matrixHeight;
        CRGB _matrixLeds[_matrixSize];
        FastLED_NeoMatrix _matrix;
        int _matrixRed = _matrix.Color(255, 0, 0);
        int _matrixGreen = _matrix.Color(0, 255, 0);
        int _matrixBlue = _matrix.Color(0, 0, 255);
        int _matrixYellow = _matrix.Color(255, 255, 0);
        int _matrixBaseX[2] = {4, 1};
        int _matrixBaseY[2] = {0, 8};
        int _matrixSemaphoreX[2] = {0, 30};

        static const int _matrixRefreshInterval = 20;
        int _lastMatrixRefresh = 0;
        void matrixRefresh();
        int _matrixBrightnessState = 0;
        int _matrixBrightness[5] = {25, 75, 125, 175, 255};
        int _lastMatrixBrightnessCicle = 0;
        void matrixBrightnessCicle();
        int _lastTimerToggle[2] = {0, 0};
        static const int _timerToggleInterval = 1000;
        void timerToggle(int timer);

        void execMsg(const String& msg);

        String getResponse();

        void idleReset();
        bool _waitPairing[4] = {false, false, false, false};
        bool semaphoreStatus(int timer);
};

#endif
