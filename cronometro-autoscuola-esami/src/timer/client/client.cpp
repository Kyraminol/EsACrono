#include "client.h"

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <Wifi.h>
#include <heltec.h>


TimerClient::TimerClient() = default;

void TimerClient::setup(String server_name, String client_name){
    if(isSetup){
        Serial.println("[Client] Already setup");
        return;
    };
    isSetup = true;
    serverName = server_name;
    clientName = client_name;
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
    http.setReuse(true);
    if(WiFi.status() == WL_CONNECTED){
        String path = endpoint + "timer?r=1";
        n == LOW ? path += "&t=0" : path += "&t=1";
        s == LOW ? path += "&s=0" : path += "&s=1";
        sendLoRa(path);
        sendRequest(path);
    }
};

void TimerClient::loop(){
    if (digitalRead(GPIO_NUM_39) == LOW){
        String path = endpoint + "timer?";
        n == LOW ? path += "t=0" : path += "t=1";
        s == LOW ? path += "&s=0" : path += "&s=1";
        Serial.println(path);
        sendRequest(path);
    }
    delay(20);
};

void TimerClient::sendRequest(String path){
    const char * headerkeys[] = {"Connection"};
    size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
    if(http.begin(client, "192.168.4.1", 80, path)){
        http.collectHeaders(headerkeys,headerkeyssize);
        int httpCode = http.GET();
        if(httpCode > 0){
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);
            if(httpCode == HTTP_CODE_OK) {
                int headers = http.headers();
                Serial.print("headers:");
                Serial.println(headers);
                for(int i=0;i<headers;i++){
                    Serial.printf("HEADER[%s]: %s\n", http.headerName(i).c_str(), http.header(i).c_str());
                }
                String payload = http.getString();
                Serial.println(payload);
            }
        } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        http.end();
    }
};

void TimerClient::sendLoRa(String msg){
    LoRa.beginPacket();
    LoRa.print(msg);
    LoRa.endPacket();
};
