#include <Arduino.h>
#include <heltec.h>

#include "src/utils/logo.h"
#include "src/timer/timer.h"

#define LORA_BAND 915E6


Timer timer;

void setup() {
    Serial.begin(115200);

    Heltec.begin(true, true, true, true, LORA_BAND);
    Heltec.display -> clear();
    Heltec.display -> drawXbm(0, 0, logo_width, logo_height, logo_bits);
    Heltec.display -> display();
    delay(2000);
    Heltec.display -> clear();
    Heltec.display -> display();

    timer.setup();
}

void loop() {
    timer.loop();
}
