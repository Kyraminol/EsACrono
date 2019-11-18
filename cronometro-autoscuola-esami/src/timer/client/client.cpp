#include "client.h"

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <Wifi.h>
#include <heltec.h>


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
    _n = digitalRead(GPIO_NUM_37);
    _s = digitalRead(GPIO_NUM_38);
    _SerialBT.begin(clientName.c_str());
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.setAutoConnect(true);
    WiFi.begin(serverName.c_str());
    for(int i = 0; i < 20 && WiFi.status() != WL_CONNECTED; i++){
        delay(500);
    }
    _http.setReuse(true);
    sendPing();
}

void TimerClient::loop(){
    if (digitalRead(GPIO_NUM_39) == LOW){
        String msg = "";
        _n == LOW ? msg += "t=0" : msg += "t=1";
        _s == LOW ? msg += "&s=0" : msg += "&s=1";
        sendLoRa(msg);
        sendRequest(_endpoint + "timer?" + msg);
    }
    sendPing();
    delay(20);
}

void TimerClient::sendRequest(String path){
    const char * headerkeys[] = {"Connection"};
    size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
    if(_http.begin(_client, "192.168.4.1", 80, path)){
        _http.collectHeaders(headerkeys,headerkeyssize);
        int httpCode = _http.GET();
        if(httpCode > 0){
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);
            if(httpCode == HTTP_CODE_OK) {
                for(int i=0;i<_http.headers();i++){
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
    Serial.println(_lastPing);
    Serial.println(millis());
    _lastPing = millis();
    String msg = "p=1";
    _n == LOW ? msg += "&t=0" : msg += "&t=1";
    _s == LOW ? msg += "&s=0" : msg += "&s=1";
    sendLoRa(msg);
    if(WiFi.status() == WL_CONNECTED){
        sendRequest(_endpoint + "timer?" + msg);
    }
}
