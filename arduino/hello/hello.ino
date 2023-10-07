#include <Arduino.h>

// setup serial
// blink 13 pin

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    pinMode(13, OUTPUT);

    digitalWrite(13, LOW);
    // delay(1000);
    // while (!Serial.available());

    Serial.println("serial is ready");
}

void loop()
{
    // put your main code here, to run repeatedly:
    Serial.println("Hello World");
    digitalWrite(13, HIGH);
    delay(1000);
    digitalWrite(13, LOW);
    delay(1000);
}
