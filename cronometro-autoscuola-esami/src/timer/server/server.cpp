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
    
    webserver.on("/api/v1/timer", HTTP_GET, [this](AsyncWebServerRequest *request){
        Serial.println("received");
        if(request->hasParam("r")){
            AsyncWebParameter* r = request->getParam("r");
            Serial.printf("GET[%s]: %s\n", r->name().c_str(), r->value().c_str());
        }
        if(request->hasParam("t")){
            AsyncWebParameter* t = request->getParam("t");
            Serial.printf("GET[%s]: %s\n", t->name().c_str(), t->value().c_str());
        }
        if(request->hasParam("s")){
            AsyncWebParameter* s = request->getParam("s");
            Serial.printf("GET[%s]: %s\n", s->name().c_str(), s->value().c_str());
        }
        request->send(200, "text/plain", "o");
    });

    webserver.on("/api/v1/timer/1/start", HTTP_GET, [this](AsyncWebServerRequest *request){
        timerStart(0);
        Serial.println("Start1");
        request->send(200, "text/plain", "START1");
    });
    webserver.on("/api/v1/timer/1/stop", HTTP_GET, [this](AsyncWebServerRequest *request){
        timerStop(0);
        String result = String(results[0], 1);
        Serial.print("Stop1: ");
        Serial.println(results[0]);
        request->send(200, "text/plain", "STOP1: " + result);
    });
    webserver.on("/api/v1/timer/1", HTTP_GET, [this](AsyncWebServerRequest *request){
        request->send(200, "text/plain", "RESULT1");
    });

    webserver.on("/api/v1/timer/2/start", HTTP_GET, [this](AsyncWebServerRequest *request){
        timerStart(1);
        Serial.println("Start2");
        request->send(200, "text/plain", "START2");
    });
    webserver.on("/api/v1/timer/2/stop", HTTP_GET, [this](AsyncWebServerRequest *request){
        timerStop(1);
        Serial.print("Stop2: ");
        Serial.println(results[1]);
        String result = String(results[1], 1);
        request->send(200, "text/plain", "Stop2: " + result);
    });
    webserver.on("/api/v1/timer/2", HTTP_GET, [this](AsyncWebServerRequest *request){
        String result = String(results[1], 1);
        request->send(200, "text/plain", result);
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
