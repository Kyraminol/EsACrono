#include "Arduino.h"
#include "heltec.h"
#include "src/utils/logo.h"
#include "src/timer/timer.h"

#define BAND 915E6

#define SERVER_SWITCH GPIO_NUM_36
#define SERVER_SWITCH_STATE HIGH

Timer timer;

void setup() {
    pinMode(SERVER_SWITCH, INPUT);

    Heltec.begin(true, true, false, true, BAND);
    Heltec.display -> clear();
    Heltec.display -> drawXbm(0, 0, logo_width, logo_height, logo_bits);
    Heltec.display -> display();
    delay(2000);
    Heltec.display -> clear();
    Heltec.display -> display();

    int buttonState = digitalRead(SERVER_SWITCH);
    if (buttonState == SERVER_SWITCH_STATE) {
        timer.init(true);
    } else {
        timer.init(false);
    }
}

void loop() {
    timer.loop();
}
