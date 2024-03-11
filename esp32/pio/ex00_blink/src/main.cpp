#include <Arduino.h>

#ifdef SEED_XIAO_ESP32C3
const int pin = D10;
#elif defined(ESP32CAM)
const int pin = 4;
#else
const int pin = BUILTIN_LED;
#endif

void setup() {
  // put your setup code here, to run once:
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(pin, LOW);
  delay(1000);
  digitalWrite(pin, HIGH);
  delay(1000);
  
}

