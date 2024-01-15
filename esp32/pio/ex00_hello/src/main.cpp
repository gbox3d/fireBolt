#include <Arduino.h>

#if defined(LOLIN_D32)
  #define LED_BUILTIN 5
  const int ledPins[] = {4,5};
#elif defined(SEED_XIAO_ESP32C3)
  const int ledPins[] = {D0, D1, D2, D3};
#elif defined(LOLIN_D32_LITE)
  const int ledPins[] = {22};
  
#endif

void setup() {

  for(int i=0; i<sizeof(ledPins)/sizeof(ledPins[0]); i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
  Serial.begin(115200);

  // Serial.println("Hello World!");
  // Serial.println(result);
}

void loop() {
  static int pinIndex = 0;
  static int delayTime = 500;
  static bool ledState[] = {false, false, false, false};
  
  digitalWrite(ledPins[pinIndex], ledState[pinIndex]);
  ledState[pinIndex] = !ledState[pinIndex];
  pinIndex = (pinIndex + 1) % (sizeof(ledPins)/sizeof(ledPins[0]));
  Serial.print("pinIndex: ");
  Serial.println(pinIndex);
  Serial.print("status of pin: ");
  Serial.println(ledState[pinIndex]);
  delay(delayTime);

  
}
