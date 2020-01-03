#include "client.h"

#include "../timer.h"

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <Wifi.h>
#include <heltec.h>
#include <FastLED.h>
#include <Wire.h>


TimerClient::TimerClient() :
    _matrix(_matrixLeds, _matrixWidth, _matrixHeight,
            NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG)
{  
}

void TimerClient::setup(String serverName, String clientName, String password, int pingInterval){
    if(_isSetup){
        Serial.println("[Client] Already setup");
        return;
    }
    _isSetup = true;
    _serverName = serverName;
    _clientName = clientName;
    _password = password;
    _pingInterval = pingInterval;
    _r = digitalRead(REMOTE_SWITCH);
    Serial.print("[Client] Starting Bluetooth...");
    _SerialBT.begin(_clientName.c_str()) ? Serial.println("ok") : Serial.println("error");
    Serial.println("[Client] Starting WiFi...");
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.begin(_serverName.c_str());
    _http.setReuse(true);
    _udp.connect(IPAddress(192,168,4,1), 404);
    _udp.onPacket([this](AsyncUDPPacket packet){
        Serial.print("[UDP] ");
        Serial.write(packet.data(), packet.length());
        Serial.println();
    });
    if(_r == HIGH){
        _t = digitalRead(TIMER_SWITCH);
        _s = digitalRead(STOP_SWITCH);

        sendPing();
        FastLED.addLeds<NEOPIXEL,LEDMATRIX_DATA>(_matrixLeds, _matrixSize).setCorrection(TypicalLEDStrip);
        _matrix.begin();
        _matrix.setBrightness(20);
        _matrix.setTextWrap(false);
    } else {
        Serial.println("ok");
        Wire.begin(17, 22);
    }
}

void TimerClient::loop(){
    if(_r == HIGH){
        if(digitalRead(START_BUTTON) == LOW){
            String msg = "";
            sendMsg(msg);
        }    
        if(digitalRead(RESET_BUTTON) == LOW){
            String msg = "r=1";
            sendMsg(msg);
        }
        sendPing();
        matrixRefresh();
    } else {
        Wire.requestFrom(0x5F, 1);
        while(Wire.available()){
            char c = Wire.read();
            if(c != 0){
                Serial.println(c, HEX);
                switch(c){
                case 0x31:
                    sendMsgRaw("t=0&s=0");
                    break;
                case 0x32:
                    sendMsgRaw("t=0&s=1");
                    break;
                case 0x33:
                    sendMsgRaw("t=1&s=0");
                    break;
                case 0x34:
                    sendMsgRaw("t=1&s=1");
                    break;
                case 0x35:
                    sendMsgRaw("t=0&s=1&r=1");
                    break;
                case 0x36:
                    sendMsgRaw("t=1&s=1&r=1");
                    break;
                default:
                    break;
                }
            }
        }
    }
    receiveLoRa();
}

void TimerClient::sendRequest(String path){
    if(WiFi.status() != WL_CONNECTED) return;
    const char * headerkeys[] = {"Connection"};
    size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
    if(_http.begin(_client, "192.168.4.1", 80, path)){
        _http.collectHeaders(headerkeys,headerkeyssize);
        int httpCode = _http.GET();
        if(httpCode > 0){
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);
            if(httpCode == HTTP_CODE_OK) {
                for(int i=0; i<_http.headers(); i++){
                    Serial.printf("HEADER[%s]: %s\n", _http.headerName(i).c_str(), _http.header(i).c_str());
                }
                String payload = _http.getString();
                Serial.println(payload);
            }
        } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", _http.errorToString(httpCode).c_str());
        }
        _http.end();
    }
}

void TimerClient::sendLoRa(String msg){
    LoRa.beginPacket();
    LoRa.print(msg);
    LoRa.endPacket(true);
}

void TimerClient::sendUDP(String msg){
    _udp.print(msg + '\0');
}

String TimerClient::getClientType(){
    String clientType = "";
    _t = digitalRead(TIMER_SWITCH);
    _s = digitalRead(STOP_SWITCH);
    clientType += _t == HIGH ? "&t=0" : "&t=1";
    clientType += _s == HIGH ? "&s=0" : "&s=1";
    return clientType;
}

void TimerClient::sendMsg(String msg, bool skipInterval){
    msg += getClientType();
    sendMsgRaw(msg, skipInterval);
}

void TimerClient::sendMsgRaw(String msg, bool skipInterval){
    if(!skipInterval && _lastMsgSent > 0 && _lastMsgSent + _msgSendInterval > millis()) return;
    _lastMsgSent = millis();
    sendLoRa(msg);
    sendUDP(msg);
}

void TimerClient::sendPing(){
    if(_lastPing > 0 && _lastPing + _pingInterval > millis()) return;
    _lastPing = millis();
    String msg = "p=1";
    sendMsg(msg, true);
}

void TimerClient::matrixRefresh(){
    if(_lastMatrixRefresh > 0 && _lastMatrixRefresh + _matrixRefreshInterval > millis()) return;
    _lastMatrixRefresh = millis();
    _matrix.drawLine(0, 0, 4, 4, _matrix.Color(255, 0, 0));
    _matrix.drawLine(4, 0, 0, 4, _matrix.Color(255, 0, 0));

    _matrix.show();
}

void TimerClient::receiveLoRa(){
    if(LoRa.parsePacket() == 0) return;

    String msg = "";
 
    while (LoRa.available()){
        msg += (char)LoRa.read();
    }
    Serial.println("[LoRa] " + msg);
}
