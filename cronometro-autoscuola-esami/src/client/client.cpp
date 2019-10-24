#include "client.h"
#include "heltec.h"
#include <WiFi.h>
#include "../utils/images/wifi.h"


TimerClient::TimerClient(){
    Heltec.display -> clear();
    Heltec.display -> display();
    WiFi.disconnect(true);
    delay(1000);
    WiFi.mode(WIFI_STA);
    WiFi.setAutoConnect(true);
    WiFi.begin("Cronometro");
    Heltec.display -> drawXbm(34, 10, wifi_logo_width, wifi_logo_height, wifi_logo_bits);
    Heltec.display -> setTextAlignment(TEXT_ALIGN_CENTER);
    Heltec.display -> drawString(64, 50, "Connessione...");
    Heltec.display -> display();
    for(int i = 0; i < 10 && WiFi.status() == WL_DISCONNECTED; i++){
        delay(500);
    }

    Heltec.display -> clear();
    Heltec.display -> drawXbm(34, 10, wifi_logo_width, wifi_logo_height, wifi_logo_bits);
    if(WiFi.status() == WL_CONNECTED){
        Heltec.display -> drawString(64, 50, "Connessa");
    } else {
        Heltec.display -> clear();
        Heltec.display -> drawString(64, 50, "Disconnessa");
    }
    Heltec.display -> display();
    delay(500);
};
