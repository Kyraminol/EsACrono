#include "server.h"

#include "../timer.h"

#include <Arduino.h>
#include <WiFi.h>
#include <BluetoothSerial.h>
#include <ESPAsyncWebServer.h>
#include <heltec.h>
#include <FastLED.h>

#include <Fonts/TomThumb.h>


TimerServer::TimerServer() :
    _webserver(80),
    _matrix(_matrixLeds, _matrixWidth / _matrixTileWidth, _matrixHeight / _matrixTileHeight, _matrixTileWidth, _matrixTileHeight, 
            NEO_TILE_TOP + NEO_TILE_RIGHT + NEO_TILE_ROWS + NEO_TILE_PROGRESSIVE +
            NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG)
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
    _udp.listen(404);

    _udp.onPacket([this](AsyncUDPPacket packet){
        String msg = String((char*)packet.data());
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
        if(pinged == false) Serial.println("[UDP] " + msg);
        if(pinged == true)
            clientPinged(timer, stop);
        else if(reset == true)
            timerReset(timer);
        else
            timerSet(timer, stop);
        packet.print("200");
    });
    _webserver.on("/api/v1/timer", HTTP_GET, [this](AsyncWebServerRequest *request){
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

        AsyncWebServerResponse *response = request->beginResponse(200);
        if(!request->hasParam("p")){
            String params = "[HTTP][GET] ";
            for(int i=0;i<request->params();i++){
                AsyncWebParameter* p = request->getParam(i);
                params += "&" + p->name() + "=" + p->value();
            }
            Serial.println(params);
        }
        
        response->addHeader("Server", "TimerServer");
        response->addHeader("Connection", "keep-alive");
        request->send(response);
    });
    _webserver.begin();
    FastLED.addLeds<NEOPIXEL,LEDMATRIX_DATA>(_matrixLeds, _matrixSize).setCorrection(TypicalLEDStrip);
    FastLED.setMaxRefreshRate(100, true);
    _matrix.begin();
    _matrix.setBrightness(_matrixBrightness[_matrixBrightnessState]);
    _matrix.setTextWrap(false);
    _matrixRed = _matrix.Color(255, 0, 0);
    _matrixGreen = _matrix.Color(0, 255, 0);
    _matrixBlue = _matrix.Color(0, 0, 255);
}

void TimerServer::loop(){
    receiveLoRa();
    pingCheck();

    if(digitalRead(RESET_BUTTON) == LOW) timerReset();
    if(digitalRead(LEDMATRIX_BRIGHTNESS_BUTTON) == HIGH) matrixBrightnessCicle();

    matrixRefresh();
}

void TimerServer::timerSet(int timer, bool stop){
    if(!stop){
        if(_timers[timer] == 0){
            _timers[timer] = millis();
        }
    } else {
        if(_timers[timer] > 0){
            _results[timer] = millis() - _timers[timer];
            _timers[timer] = 0;
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
    if(pinged == false) Serial.println("[LoRa] " + msg);
    if(pinged == true)
        clientPinged(timer, stop);
    else if(reset == true)
        timerReset(timer);
    else
        timerSet(timer, stop);
};

void TimerServer::clientPinged(int timer, bool stop){
    int n = timer == 0 ? 0 + stop : 2 + stop;
    if(_pings[n] == 0){
        String toPrint = "[CLIENT]";
        toPrint += timer == 0 ? "[T0]" : "[T1]";
        toPrint += stop == false ? "[START]" : "[STOP]";
        Serial.println(toPrint + " Connected");
    }
    _pings[n] = millis();
}

void TimerServer::pingCheck(){
    if(_lastPingCheck > 0 && _lastPingCheck + _pingInterval > millis()) return;
    _lastPingCheck = millis();
    for(int i = 0; i < 4; i++){
        if(_pings[i] > 0 && _pings[i] + _pingInterval + 500 < millis()){
            _pings[i] = 0;
            String toPrint = "[CLIENT]";
            toPrint += i / 2 == 0 ? "[T0]" : "[T1]";
            toPrint += i % 2 == 0 ? "[START]" : "[STOP]";
            Serial.println(toPrint + " Disconnected");
        }
    }
}

void TimerServer::matrixRefresh(){
    if(_lastMatrixRefresh > 0 && _lastMatrixRefresh + _matrixRefreshInterval > millis()) return;
    _lastMatrixRefresh = millis();
    
    int matrixSwitch = digitalRead(LEDMATRIX_SWITCH);
    matrixSwitch == LOW ? _matrix.setFont() : _matrix.setFont(&TomThumb);
    _matrix.fillScreen(0);

    String minutes = "0";
    String seconds = "00";
    String decimals = "0";
    int color = _matrixBlue;

    if(_timers[0] > 0 || _results[0] > 0){
        color = _matrixRed;
        int d =  0;
        if(_timers[0] > 0)
            d = (millis() - _timers[0]) / 100;
        else
            d = _results[0] / 100;
        int s = d / 10;
        int m = s / 60;
        s -= m * 60;
        if(m > 0 || s > 15 || (s > 14 && d > 1)) color = _matrixGreen;
        minutes = String(m);
        s < 10 ? seconds = "0" + String(s) : seconds = String(s);
        decimals = String(d);
        decimals = decimals.substring(decimals.length() - 1);
    }
    
    _matrix.setTextColor(color);
    if(matrixSwitch == LOW){
        _matrix.setCursor(0, 0);
        _matrix.print(minutes);
        _matrix.setCursor(5, 0);
        _matrix.print(":");
        _matrix.setCursor(10, 0);
        _matrix.print(seconds);
        _matrix.setCursor(21, 0);
        _matrix.print(":");
        _matrix.setCursor(26, 0);
        _matrix.print(decimals);
        _pings[0] == 0 ? _matrix.fillRect(6, 6, 3, 2, _matrixRed) : _matrix.fillRect(6, 6, 3, 2, _matrixGreen);
        _pings[1] == 0 ? _matrix.fillRect(22, 6, 3, 2, _matrixRed) : _matrix.fillRect(22, 6, 3, 2, _matrixGreen);
    } else {
        _matrix.setCursor(1, 6);
        _matrix.print(seconds + ":" + decimals);
        _pings[0] == 0 ? _matrix.drawFastHLine(2, 7, 4, _matrixRed) : _matrix.drawFastHLine(2, 7, 4, _matrixGreen);
        _pings[1] == 0 ? _matrix.drawFastHLine(9, 7, 4, _matrixRed) : _matrix.drawFastHLine(9, 7, 4, _matrixGreen);
        _matrix.drawFastVLine(15, 0, 8, _matrix.Color(0, 255, 255));
        _matrix.drawFastVLine(16, 0, 8, _matrix.Color(0, 255, 255));
    }

    minutes = "0";
    seconds = "00";
    decimals = "0";
    color = _matrixBlue;
    if(_timers[1] > 0 || _results[1] > 0){
        color = _matrixGreen;
        int d = 0;
        if(_timers[1] > 0)
            d = (millis() - _timers[1]) / 100;
        else
            d = _results[1] / 100;
        int s = d / 10;
        int m = s / 60;
        s -= m * 60;
        if(m > 0 || s > 25) color = _matrixRed;
        minutes = String(m);
        s < 10 ? seconds = "0" + String(s) : seconds = String(s);
        decimals = String(d);
        decimals = decimals.substring(decimals.length() - 1);
    }
    _matrix.setTextColor(color);
    if(matrixSwitch == LOW){
        _matrix.setCursor(0, 8);
        _matrix.print(minutes);
        _matrix.setCursor(5, 8);
        _matrix.print(":");
        _matrix.setCursor(10, 8);
        _matrix.print(seconds);
        _matrix.setCursor(21, 8);
        _matrix.print(":");
        _matrix.setCursor(26, 8);
        _matrix.print(decimals);
        _pings[2] == 0 ? _matrix.fillRect(6, 14, 3, 2, _matrixRed) : _matrix.fillRect(6, 14, 3, 2, _matrixGreen);
        _pings[3] == 0 ? _matrix.fillRect(22, 14, 3, 2, _matrixRed) : _matrix.fillRect(22, 14, 3, 2, _matrixGreen);
    } else {
        _matrix.setCursor(18, 6);
        _matrix.print(seconds + ":" + decimals);
        _pings[2] == 0 ? _matrix.drawFastHLine(19, 7, 4, _matrixRed) : _matrix.drawFastHLine(19, 7, 4, _matrixGreen);
        _pings[3] == 0 ? _matrix.drawFastHLine(26, 7, 4, _matrixRed) : _matrix.drawFastHLine(26, 7, 4, _matrixGreen);
    }

    _matrix.show();
}

void TimerServer::matrixBrightnessCicle(){
    if(_lastMatrixBrightnessCicle > 0 && _lastMatrixBrightnessCicle + 1000 > millis()) return;
    _lastMatrixBrightnessCicle = millis();

    _matrixBrightnessState == 4 ? _matrixBrightnessState = 0 : _matrixBrightnessState++;
    
    _matrix.setBrightness(_matrixBrightness[_matrixBrightnessState]);
}
