#include "server.h"

#include "../timer.h"

#include <Arduino.h>
#include <WiFi.h>
#include <BluetoothSerial.h>
#include <ESPAsyncWebServer.h>
#include <heltec.h>


TimerServer::TimerServer() :
    _webserver(80),
    _matrix(32, 8, 1, 2, LEDMATRIX_DATA,
            NEO_TILE_TOP + NEO_TILE_RIGHT + NEO_TILE_ROWS + NEO_TILE_PROGRESSIVE +
            NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
            NEO_GRB + NEO_KHZ800)
{  
}

void TimerServer::setup(String serverName, String clientName, int pingInterval){
    if(_isSetup){
        Serial.println("[Server] Already setup");
        return;
    }
    _isSetup = true;
    _serverName = serverName;
    _clientName = clientName;
    _pingInterval = pingInterval;
    _SerialBT.begin(_serverName.c_str());
    WiFi.mode(WIFI_AP);
    WiFi.softAP(_serverName.c_str());
    
    _webserver.on("/api/v1/timer", HTTP_GET, [this](AsyncWebServerRequest *request){
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
        
        if(request->hasParam("p"))
            clientPinged(timer, stop);
        else if(request->hasParam("r"))
            timerReset(timer);
        else
            timerSet(timer, stop);

        String responseText = String(_results[timer], 1);
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", responseText);
        response->addHeader("Server", "TimerServer");
        response->addHeader("Connection", "keep-alive");
        request->send(response);
    });
    
    _webserver.begin();
    _matrix.begin();
    _matrix.setBrightness(50);
    _matrix.setTextWrap(false);
}

void TimerServer::loop(){
    receiveLoRa();
    pingCheck();
    if(digitalRead(GPIO_NUM_39) == LOW){
        Serial.print("Timer 1: ");
        Serial.println(_timers[0]);
        Serial.println(_results[0]);
        Serial.print("Timer 2: ");
        Serial.println(_timers[1]);
        Serial.println(_results[1]);
    }
    if(digitalRead(RESET_BUTTON) == LOW){
        timerReset();
    }
    matrixRefresh();
}

void TimerServer::timerSet(int timer, bool stop){
    if(!stop){
        if(_timers[timer] == 0){
            _timers[timer] = millis();
        }
    } else {
        if(_timers[timer] > 0){
            _results[timer] = (float)(millis() - _timers[timer]) / 1000.0;
            _timers[timer] = 0;
            Serial.println(_results[timer]);
        }
    }
}

void TimerServer::timerReset(int timer){
    if(timer == -1){
        for(int i=0; i < 2; i++){
            _timers[i] = 0;
            _results[i] = 0;
        }
    } else {
        _timers[timer] = 0;
        _results[timer] = 0;
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

    int timer = -1;
    bool stop = false, pinged = false, reset = false;
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
        else if(name == "p")
            value == "0" ? pinged = false : pinged = true;
        else if(name == "r")
            value == "0" ? reset = false : reset = true;
    }
    Serial.println("Timer: " + String(timer));
    Serial.println("Stop: " + String(stop));
    Serial.println("Pinged: " + String(pinged));
    if(pinged == true)
        clientPinged(timer, stop);
    else if(reset == true)
        timerReset(timer);
    else
        timerSet(timer, stop);
};

void TimerServer::clientPinged(int timer, bool stop){
    int n = 0;
    timer == 0 ? n = 0 + stop : n = 2 + stop;
    _pings[n] = millis();
}

void TimerServer::pingCheck(){
    if(_lastPingCheck > 0 && _lastPingCheck + _pingInterval > millis()) return;
    _lastPingCheck = millis();
    for(int i = 0; i < 4; i++){
        if(_pings[i] > 0 && _pings[i] + _pingInterval + 200 < millis()){
            _pings[i] = 0;
            Serial.println("Client disconnected: " + String(i));
        }
    }
}

void TimerServer::matrixRefresh(){
    if(_lastMatrixRefresh > 0 && _lastMatrixRefresh + 100 > millis()) return;
    _lastMatrixRefresh = millis();
    _matrix.fillScreen(0);
    _matrix.setTextColor(_matrix.Color(0, 255, 0));

    _matrix.setCursor(0, 0);
    _matrix.print("0");

    _matrix.setCursor(5, 0);
    _matrix.print(":");

    _matrix.setCursor(10, 0);
    _matrix.print("00");

    _matrix.setCursor(21, 0);
    _matrix.print(":");

    _matrix.setCursor(26, 0);
    _matrix.print("0");

    _matrix.setTextColor(_matrix.Color(255, 0, 0));

    _matrix.setCursor(0, 8);
    _matrix.print("0");

    _matrix.setCursor(5, 8);
    _matrix.print(":");

    _matrix.setCursor(10, 8);
    _matrix.print("00");

    _matrix.setCursor(21, 8);
    _matrix.print(":");

    _matrix.setCursor(26, 8);
    _matrix.print("0");
    
    _matrix.show();
}
