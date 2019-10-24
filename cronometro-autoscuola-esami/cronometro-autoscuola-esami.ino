#include "Arduino.h"
#include "heltec.h"
#include "src/utils/images/logo.h"
#include "src/server/server.h"
#include "src/client/client.h"

#define BAND 868E6


void setup() {
  Heltec.begin(true, true, true, true, BAND);
  Heltec.display -> clear();
  Heltec.display -> drawXbm(0, 0, logo_width, logo_height, logo_bits);
  Heltec.display -> display();
  delay(2000);
  Heltec.display -> clear();
  Heltec.display -> display();

  int buttonState = digitalRead(GPIO_NUM_0);
  if (buttonState == LOW) {
    TimerServer server;
  } else {
    TimerClient client;
  }
  delay(300);
  
}

void loop() {
}
