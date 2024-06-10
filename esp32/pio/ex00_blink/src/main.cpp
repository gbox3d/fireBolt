#include <Arduino.h>

#ifdef SEED_XIAO_ESP32C3
const int pin = D10;
const int colorLeds[] = {D3, D4, D5};
#elif defined(ESP32CAM)
const int pin = 4;
#elif defined(WROVER_KIT)
const int pin = 5;
#else
const int pin = BUILTIN_LED;
#endif

void setup()
{
  // put your setup code here, to run once:
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);

#ifdef SEED_XIAO_ESP32C3
  for (int i = 0; i < 3; i++)
  {
    pinMode(colorLeds[i], OUTPUT);
    digitalWrite(colorLeds[i], LOW);
  }
#endif
}

void loop()
{
  // put your main code here, to run repeatedly:

#ifdef SEED_XIAO_ESP32C3
  for (int i = 0; i < 3; i++)
  {
    digitalWrite(colorLeds[i], HIGH);
    delay(1000);
  }

  for (int i = 0; i < 3; i++)
  {
    digitalWrite(colorLeds[i], LOW);
    delay(1000);
  }

#else
  digitalWrite(pin, LOW);
  delay(1000);
  digitalWrite(pin, HIGH);
  delay(1000);

#endif
}
