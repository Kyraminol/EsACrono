#include "timer.h"

#include <Arduino.h>
#include <BluetoothSerial.h>


Timer::Timer() = default;

void Timer::setup(){
    if(_isSetup){
        Serial.println("[Timer] Already setup");
        return;
    };
    _isSetup = true;

    pinMode(SERVER_SWITCH, INPUT);
    pinMode(TIMER_SWITCH, INPUT);
    pinMode(STOP_SWITCH, INPUT);
    pinMode(REMOTE_SWITCH, INPUT);
    pinMode(START_BUTTON, INPUT);
    pinMode(RESET_BUTTON, INPUT);
    pinMode(LEDMATRIX_SWITCH, INPUT);
    pinMode(LEDMATRIX_DATA, OUTPUT);
    pinMode(LEDMATRIX_BRIGHTNESS_BUTTON, INPUT);
    pinMode(RESISTOR_PASSWORD, INPUT);

    String password = "";
    int resistorValue = analogRead(RESISTOR_PASSWORD);

    int serverSwitch = digitalRead(SERVER_SWITCH);
    if(serverSwitch == LOW){
        Serial.println("[Timer] Starting as server...");
        _isServer = true;
        _server.setup(_serverName, _clientName, password, _pingInterval, _LoRaMsgSize);
    } else {
        Serial.println("[Timer] Starting as client...");
        _client.setup(_serverName, _clientName, password, _pingInterval, _LoRaMsgSize);
    }
    Serial.println("[Timer] Setup done");
};

void Timer::loop(){
    _isServer ? _server.loop() : _client.loop();
};


void parseMsg(LinkedList<RequestParameter *>& params, const String& msg){
    size_t start = 0;
    while (start < msg.length()){
        int end = msg.indexOf('&', start);
        if (end < 0) end = msg.length();
        int equal = msg.indexOf('=', start);
        if (equal < 0 || equal > end) equal = end;
        String name = msg.substring(start, equal);
        String value = equal + 1 < end ? msg.substring(equal + 1, end) : String();
        RequestParameter *param = new RequestParameter(name, value);
        params.add(param);
        start = end + 1;
    }
}

RequestParameter* getParam(const LinkedList<RequestParameter *>& params, const String& name){
  for(const auto& p: params){
    if(p->name() == name){
      return p;
    }
  }
  return nullptr;
}

bool hasParam(const LinkedList<RequestParameter *>& params, const String& name){
  for(const auto& p: params){
    if(p->name() == name){
      return true;
    }
  }
  return false;
}
