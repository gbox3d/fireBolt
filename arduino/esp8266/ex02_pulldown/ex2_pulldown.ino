#include <ESP8266WiFi.h>

const int pulldown_pin = 16; //d0
const int led_pin = 14; //d5

void setup()
{
    //16 핀은 pulldown 가능하다, 나머지 핀들은 pullup 만 가능함.
    pinMode(pulldown_pin,INPUT_PULLDOWN_16);
    pinMode(led_pin,OUTPUT);
    digitalWrite(led_pin,LOW);
}

void loop()
{
    if( digitalRead(pulldown_pin)  ) {
        digitalWrite(led_pin,HIGH);
    }
    else {
         digitalWrite(led_pin,LOW);
    }

}