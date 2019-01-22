#include <Arduino.h>

void setup()
{

    pinMode(2, INPUT_PULLUP);

    Serial.begin(115200);
}

void loop()
{   
    Serial.println(digitalRead(2));
    delay(100);
}