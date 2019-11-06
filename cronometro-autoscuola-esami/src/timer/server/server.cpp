#include "server.h"

#include <Arduino.h>
#include <WiFi.h>
#include <BluetoothSerial.h>
#include <ESPAsyncWebServer.h>


TimerServer::TimerServer() : webserver(80){  
};

void TimerServer::setup(String server, String client){
    if(isSetup){
        Serial.println("[Server] Already setup");
        return;
    };
    isSetup = true;
    serverName = server;
    clientName = client;
    SerialBT.begin(serverName.c_str());
    WiFi.softAP(serverName.c_str());
    
    webserver.on("/api/v1/timer/1/start", HTTP_GET, [this](AsyncWebServerRequest *request){
        timerStart(0);
        request->send(200, "text/plain", "START1");
    });
    webserver.on("/api/v1/timer/1/stop", HTTP_GET, [this](AsyncWebServerRequest *request){
        timerStart(0);
        request->send(200, "text/plain", "STOP1");
    });
    webserver.on("/api/v1/timer/1", HTTP_GET, [this](AsyncWebServerRequest *request){
        timerStart(0);
        request->send(200, "text/plain", "RESULT1");
    });

    webserver.on("/api/v1/timer/2/start", HTTP_GET, [this](AsyncWebServerRequest *request){
        timerStart(1);
        request->send(200, "text/plain", "START2");
    });
    webserver.on("/api/v1/timer/2/stop", HTTP_GET, [this](AsyncWebServerRequest *request){
        timerStop(1);
        request->send(200, "text/plain", "STOP2");
    });
    webserver.on("/api/v1/timer/2", HTTP_GET, [this](AsyncWebServerRequest *request){
        request->send(200, "text/plain", "RESULT2");
    });
    
    webserver.begin();
};

void TimerServer::loop(){

};

void TimerServer::timerStart(int timer){
    if(timers[timer] == 0){
        timers[timer] = millis();
    };
};

void TimerServer::timerStop(int timer){
    if(timers[timer] > 0){
        results[timer] = (float)(millis() - timers[timer]) / 1000.0;
        timers[timer] = 0;
        Serial.println(results[timer]);
    };
};


