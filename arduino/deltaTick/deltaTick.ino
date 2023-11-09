//miils 함수사 오버 플로우 발생시 delta tick 계산을 위한 함수

#include <Arduino.h>
#include <limits.h>

unsigned long deltaTick(unsigned long now, unsigned long last)
{
  if (now >= last)
  {
    return now - last;
  }
  else
  {
    return (UINT_MAX - last) + now;
  }
}

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  static unsigned long last = 0;
  unsigned long now = millis();
  unsigned long delta = deltaTick(now, last);
  Serial.print("now: ");
  Serial.print(now);
  Serial.print(", last: ");
  Serial.print(last);
  Serial.print(", delta: ");
  Serial.println(delta);
  last = now;
  delay(1000);
}