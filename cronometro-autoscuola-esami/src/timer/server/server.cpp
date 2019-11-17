#include "server.h"

#include <Arduino.h>
#include <WiFi.h>
#include <BluetoothSerial.h>
#include <ESPAsyncWebServer.h>
#include <heltec.h>


TimerServer::TimerServer() : webserver(80){  
}

void TimerServer::setup(String server, String client){
    if(isSetup){
        Serial.println("[Server] Already setup");
        return;
    }
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
        int params = request->params();
        for(int i=0;i<params;i++){
            AsyncWebParameter* p = request->getParam(i);
            Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }

        if(!request->hasParam("t") && !request->hasParam("s")){
            request->send(400);
            return;
        }
        
        int timer;
        bool stop;
        request->getParam("t")->value() == "0" ? timer = 0 : timer = 1;
        request->getParam("s")->value() == "0" ? stop = false : stop = true;
        
        request->hasParam("r") ? clientRegister(timer, stop) : timerSet(timer, stop);

        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Ok");
        response->addHeader("Server", "TimerServer");
        response->addHeader("Connection", "keep-alive");
        request->send(response);
    });
    
    webserver.begin();
}

void TimerServer::loop(){
    receiveLoRa();
}

void TimerServer::timerSet(int timer, bool stop){
    if(!stop){
        if(timers[timer] == 0){
            timers[timer] = millis();
        }
    } else {
        if(timers[timer] > 0){
            results[timer] = (float)(millis() - timers[timer]) / 1000.0;
            timers[timer] = 0;
            Serial.println(results[timer]);
        }
    }
}

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

    int timer = -1;
    bool stop = false, registerClient = false;
    size_t start = 0;
    while (start < msg.length()){
        int end = msg.indexOf('&', start);
        if (end < 0) end = msg.length();
        int equal = msg.indexOf('=', start);
        if (equal < 0 || equal > end) equal = end;
        String name = msg.substring(start, equal);
        String value = equal + 1 < end ? msg.substring(equal + 1, end) : String();
        start = end + 1;
        if(name == "t")
            value == "0" ? timer = 0 : timer = 1;
        else if(name == "s")
            value == "0" ? stop = false : stop = true;
        else if(name == "r")
            value == "0" ? registerClient = false : registerClient = true;
    }

    registerClient == true ? clientRegister(timer, stop) : timerSet(timer, stop);
};

void TimerServer::clientRegister(int timer, bool stop){

}
