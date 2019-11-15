#include "server.h"

#include <Arduino.h>
#include <WiFi.h>
#include <BluetoothSerial.h>
#include <ESPAsyncWebServer.h>
#include <heltec.h>


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
        int headers = request->headers();
        for(int i=0;i<headers;i++){
            AsyncWebHeader* h = request->getHeader(i);
            Serial.printf("HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
        }
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
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Ok");
        response->addHeader("Server", "ESP Async Web Server");
        response->addHeader("Connection", "keep-alive");
        request->send(response);
    });
    
    webserver.begin();
};

void TimerServer::loop(){
    receiveLoRa();
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

void TimerServer::receiveLoRa(){
    if(LoRa.parsePacket() == 0) return;

    String msg = "";
 
    while (LoRa.available()){
        msg += (char)LoRa.read();
    }

    Serial.println("-- LoRa --");
    Serial.println("Message: " + msg);
    Serial.println("RSSI: " + String(LoRa.packetRssi()));
    Serial.println("Snr: " + String(LoRa.packetSnr()));
    Serial.println();
};
