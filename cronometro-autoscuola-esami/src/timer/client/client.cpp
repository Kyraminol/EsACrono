#include "client.h"

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <Wifi.h>
#include <heltec.h>


#define TIMER_N_PIN GPIO_NUM_37
#define START_STOP_SWITCH_PIN GPIO_NUM_38
#define START_STOP_BUTTON_PIN GPIO_NUM_39


TimerClient::TimerClient() = default;

void TimerClient::setup(String serverName, String clientName, int pingInterval){
    if(_isSetup){
        Serial.println("[Client] Already setup");
        return;
    }
    _isSetup = true;
    _serverName = serverName;
    _clientName = clientName;
    _pingInterval = pingInterval;
    _t = digitalRead(TIMER_N_PIN);
    _s = digitalRead(START_STOP_SWITCH_PIN);
    _SerialBT.begin(_clientName.c_str());
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.begin(_serverName.c_str());
    for(int i = 0; i < 20 && WiFi.status() != WL_CONNECTED; i++){
        delay(500);
    }
    _http.setReuse(true);
    sendPing();
}

void TimerClient::loop(){
    _t = digitalRead(TIMER_N_PIN);
    _s = digitalRead(START_STOP_SWITCH_PIN);
    if(digitalRead(START_STOP_BUTTON_PIN) == LOW){
        String msg = "";
        _t == LOW ? msg += "t=0" : msg += "t=1";
        _s == LOW ? msg += "&s=0" : msg += "&s=1";
        sendLoRa(msg);
        sendRequest(_endpoint + "timer?" + msg);
    }
    sendPing();
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
    LoRa.endPacket();
}

void TimerClient::sendPing(){
    if(_lastPing > 0 && _lastPing + _pingInterval > millis()) return;
    _lastPing = millis();
    String msg = "p=1";
    _t == LOW ? msg += "&t=0" : msg += "&t=1";
    _s == LOW ? msg += "&s=0" : msg += "&s=1";
    sendLoRa(msg);
    sendRequest(_endpoint + "timer?" + msg);
}
