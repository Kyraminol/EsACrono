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
    SerialBT.begin(clientName.c_str());
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.setAutoConnect(true);
    WiFi.begin(serverName.c_str());
    for(int i = 0; i < 10 && WiFi.status() == WL_DISCONNECTED; i++){
        delay(500);
    }
};

void TimerClient::loop(){
    if (digitalRead(GPIO_NUM_39) == LOW){
        digitalWrite(25,HIGH);
        int n = digitalRead(GPIO_NUM_37);
        int s = digitalRead(GPIO_NUM_38);
        String path = "/api/v1/timer/";
        n == LOW ? path += "1/" : path += "2/";
        s == LOW ? path += "start" : path += "stop";
        Serial.println(path);
        sendRequest(path);
        delay(1000);
        digitalWrite(25,LOW);
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
    client.end();
};
