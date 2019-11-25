#ifndef TIMERSERVER_H
#define TIMERSERVER_H

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_NeoMatrix.h>


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
        Adafruit_NeoMatrix _matrix;
        int _lastMatrixRefresh = 0;
        void matrixRefresh();
};

#endif
