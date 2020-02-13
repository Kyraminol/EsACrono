#include "server.h"

#include "../timer.h"

#include <Arduino.h>
#include <WiFi.h>
#include <BluetoothSerial.h>
#include <heltec.h>
#include <FastLED.h>
#include <SPIFFS.h>

#include <Fonts/TomThumb.h>


TimerServer::TimerServer() :
    _webserver(80),
    _matrix(_matrixLeds, _matrixWidth / _matrixTileWidth, _matrixHeight / _matrixTileHeight, _matrixTileWidth, _matrixTileHeight, 
            NEO_TILE_TOP + NEO_TILE_RIGHT + NEO_TILE_ROWS + NEO_TILE_PROGRESSIVE +
            NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG)
{  
}

void TimerServer::setup(String serverName, String clientName, String password, int pingInterval){
    if(_isSetup){
        Serial.println("[Server] Already setup");
        return;
    }
    _isSetup = true;
    _serverName = serverName;
    _clientName = clientName;
    _password = password;
    _pingInterval = pingInterval;
    
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
  }

 
    _SerialBT.begin(_serverName.c_str());
    WiFi.mode(WIFI_AP);
    WiFi.softAP(_serverName.c_str(), NULL, 1, 0, 8);
    _udp.listen(404);

    _udp.onPacket([this](AsyncUDPPacket packet){
        String msg = String((char*)packet.data());
        execMsg(msg);
        packet.print(getResponse() + '\0');
    });
    _webserver.serveStatic("/", SPIFFS, "/index.html");
    _webserver.on("/api/v1/timer", HTTP_GET, [this](){
        String msg = "";
        
        for(int i = 0; i < _webserver.args(); i++){
            if(i != 0)
                msg += "&";
            msg += _webserver.argName(i) + "=" + _webserver.arg(i);
        }

        execMsg(msg);

        _webserver.sendHeader("Connection", "keep-alive");
        _webserver.sendHeader("Server", "TimerServer");
        _webserver.send(200, "text/plain", getResponse());
    });
    _webserver.begin();
    FastLED.addLeds<NEOPIXEL,LEDMATRIX_DATA>(_matrixLeds, _matrixSize).setCorrection(TypicalLEDStrip);
    _matrix.begin();
    _matrix.setBrightness(_matrixBrightness[_matrixBrightnessState]);
    _matrix.setTextWrap(false);
}

void TimerServer::loop(){
    _webserver.handleClient();
    pingCheck();

    if(digitalRead(SERVER_T0_START) == LOW) timerToggle(0);
    if(digitalRead(SERVER_T1_START) == LOW) timerToggle(1);
    if(digitalRead(RESET_BUTTON) == LOW) timerReset();
    if(digitalRead(LEDMATRIX_BRIGHTNESS_BUTTON) == LOW) matrixBrightnessCicle();

    idleReset();

    matrixRefresh();
}

void TimerServer::timerSet(int timer, bool stop, bool ignoreMinInterval){
    if(!stop){
        if(_timers[timer] == 0){
            _timers[timer] = millis();
            _stopped[timer] = 0;
        }
    } else {
        if(!ignoreMinInterval && millis() - _timers[timer] < 5000) return;
        if(_timers[timer] > 0){
            _results[timer] = millis() - _timers[timer];
            _stopped[timer] = millis();
            _timers[timer] = 0;
        }
    }
}

void TimerServer::timerReset(int timer){
    if(timer == -1){
        for(int i=0; i < 2; i++){
            _timers[i] = 0;
            _results[i] = 0;
            _stopped[i] = 0;
        }
    } else {
        _timers[timer] = 0;
        _results[timer] = 0;
        _stopped[timer] = 0;
    }
}

void TimerServer::clientPinged(int timer, bool stop, bool waiting){
    int n = timer == 0 ? 0 + stop : 2 + stop;
    if(_pings[n] == 0){
        String toPrint = "[CLIENT]";
        toPrint += timer == 0 ? "[T0]" : "[T1]";
        toPrint += stop == false ? "[START]" : "[STOP]";
        Serial.println(toPrint + " Connected");
    }
    _pings[n] = millis();
    _waitPairing[n] = waiting;
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

    for(int i=0; i < 2; i++){
        int m = 0;
        int s = 0;
        int d = 0;
        int color = _matrixBlue;

        if(_timers[i] > 0 || _results[i] > 0){
            if(_timers[i] > 0)
                d = (millis() - _timers[i]) / 100;
            else
                d = _results[i] / 100;
            s = d / 10;
            m = s / 60;
            s -= m * 60;
            if(i == 0)
                color = m > 0 || s > 15 || (s > 14 && d > 1) ? _matrixGreen : _matrixRed;
            else
                color = m > 0 || s > 25 ? _matrixRed : _matrixGreen;
        }

        String temp = String(d);
        String t[5] = {String(m), ":", s < 10 ? "0" + String(s) : String(s), ":", temp.substring(temp.length() - 1)};
        _matrix.setTextColor(color);

        if(matrixSwitch == LOW){
            int x = _matrixBaseX[i];
            int y = _matrixBaseY[i];
            _matrix.setCursor(x+2, y);
            for(int k = 0; k < 5; k++){
                int cursor = _matrix.getCursorX();
                if(k % 2 == 1){
                    int statusColor = _matrixRed;
                    int n = i == 0 ? 0 : 2;
                    if(k == 3) n += 1;
                    if(_pings[n] > 0)
                        statusColor = _waitPairing[n] ? _matrixYellow : _matrixGreen;
                    _matrix.drawFastHLine(cursor-1, y+7, 3, statusColor);
                    _matrix.drawFastVLine(cursor, y+6, 2, statusColor);
                }
                _matrix.setCursor(cursor-2, y);
                _matrix.print(t[k]);
            }
            int semaphoreColor = semaphoreStatus(i) ? _matrixGreen : _matrixRed;
            _matrix.fillRect(_matrixSemaphoreX[i], y, 2, 8, semaphoreColor);
        } else {

        }
    }

    _matrix.show();
}

void TimerServer::matrixBrightnessCicle(){
    if(_lastMatrixBrightnessCicle > 0 && _lastMatrixBrightnessCicle + 200 > millis()) return;
    _lastMatrixBrightnessCicle = millis();

    _matrixBrightnessState == 4 ? _matrixBrightnessState = 0 : _matrixBrightnessState++;
    
    _matrix.setBrightness(_matrixBrightness[_matrixBrightnessState]);
}

void TimerServer::timerToggle(int timer){
    if(_lastTimerToggle[timer] > 0 && _lastTimerToggle[timer] + _timerToggleInterval > millis()) return;
    _lastTimerToggle[timer] = millis();
    _timers[timer] == 0 ? timerSet(timer, false) : timerSet(timer, true, true);
}

void TimerServer::execMsg(const String& msg){
    LinkedList<RequestParameter *> params([](RequestParameter *p){ delete p;});
    parseMsg(params, msg);

    if(hasParam(params, "t")){
        int timer = getParam(params, "t")->value() == "0" ? 0 : 1;
        if(hasParam(params, "r")) timerReset(timer);
        else {
            if(hasParam(params, "s")){
                bool stop = getParam(params, "s")->value() == "0" ? false : true;
                bool waiting = hasParam(params, "w") && getParam(params, "w")->value() == "1" ? true : false;
                if(hasParam(params, "p")) clientPinged(timer, stop, waiting); 
                else timerSet(timer, stop);
            } else timerToggle(timer);
        }
    }

    params.free();
}

String TimerServer::getResponse(){
    String response = "b=" + String(_matrixBrightness[_matrixBrightnessState]);

    for(int i = 0; i < 2; i++){
        if(response != "")
            response += "&";
        String n = String(i);
        String status = semaphoreStatus(i) ? "1" : "0";
        response += "s" + n + "=" + status;

        if(_timers[i] > 0){
            response += "&t" + n + "=" + String(millis() - _timers[0]);
            response += "&r" + n + "=1";
        } else {
            response += "&t" + n + "=" + String(_results[0]);
            response += "&r" + n + "=0";
        }

    }

    return response;
}

void TimerServer::idleReset(){
    for(int i = 0; i < 2; i++){
        if(_stopped[i] == 0) continue;
        if(millis() - _stopped[i] > 300000) timerReset(i);
    }
}

bool TimerServer::semaphoreStatus(int timer){
    if(_timers[timer] > 0) return false;
    int t = timer == 0 ? 0 : 2;
    if(_pings[t] == 0 || _pings[t + 1] == 0 || _waitPairing[t] || _waitPairing[t + 1]) return false;
    if(_results[timer] == 0 || (millis() - _stopped[timer]) > 8000) return true;
    return false;
}
