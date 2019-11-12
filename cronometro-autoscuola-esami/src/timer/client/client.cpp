#include "client.h"

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <Wifi.h>
#include <HTTPClient.h>


TimerClient::TimerClient() = default;

void TimerClient::setup(String server, String client){
    if(isSetup){
        Serial.println("[Client] Already setup");
        return;
    };
    isSetup = true;
    serverName = server;
    clientName = client;
    n = digitalRead(GPIO_NUM_37);
    s = digitalRead(GPIO_NUM_38);
    SerialBT.begin(clientName.c_str());
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.setAutoConnect(true);
    WiFi.begin(serverName.c_str());
    for(int i = 0; i < 20 && WiFi.status() != WL_CONNECTED; i++){
        delay(500);
    }
    if(WiFi.status() == WL_CONNECTED){
        Serial.println("send1");
        String path = endpoint + "timer?r=1";
        n == LOW ? path += "&t=0" : path += "&t=1";
        s == LOW ? path += "&s=0" : path += "&s=1";
        sendRequest(path);
    }
};

void TimerClient::loop(){
    if (digitalRead(GPIO_NUM_39) == LOW){
        String path = endpoint + "timer?r=1";
        n == LOW ? path += "&t=0" : path += "&t=1";
        s == LOW ? path += "&s=0" : path += "&s=1";
        Serial.println(path);
        sendRequest(path);
        delay(1000);
    }
    delay(20);
};

void TimerClient::sendRequest(String path){
    if(WiFi.status() != WL_CONNECTED){
        Serial.println("[HTTP] [ERROR] WiFi not connected");
        return;
    }
    HTTPClient client;
    client.begin("http://192.168.4.1" + path);
    int httpCode = client.GET();
    if(httpCode > 0){
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
        if(httpCode == HTTP_CODE_OK) {
            String payload = client.getString();
            Serial.println(payload);
        }
    } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", client.errorToString(httpCode).c_str());
    }
    delay(100);
    client.end();
};
