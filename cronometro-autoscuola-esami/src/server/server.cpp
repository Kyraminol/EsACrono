#include "server.h"
#include "heltec.h"
#include <WiFi.h>
#include "../utils/images/wifi.h"


TimerServer::TimerServer() {
    char APName[] = "Cronometro";

    Serial.begin(115200);
    WiFi.softAP(APName);
    webserver = WiFiServer(80);
    webserver.begin();
    Heltec.display -> clear();
    Heltec.display -> drawXbm(0, 14, wifi_logo_width, wifi_logo_height, wifi_logo_bits);

    Heltec.display -> drawString(60, 0, "---1a prova---");
    Heltec.display -> drawString(60, 30, "---2a prova---");
    Heltec.display -> setTextAlignment(TEXT_ALIGN_RIGHT);
    Heltec.display -> drawString(110, 10, "Inizio: NO");
    Heltec.display -> drawString(110, 20, "Fine: NO");
    Heltec.display -> drawString(110, 40, "Inizio: NO");
    Heltec.display -> drawString(110, 50, "Fine: NO");
    Heltec.display -> display();
};
