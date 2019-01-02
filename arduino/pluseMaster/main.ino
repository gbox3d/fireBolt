#include <Arduino.h>

const int g_DisplayPin= 13;
const int g_InputPin = 3;
const int g_HLPulsePin = 5;
const int g_LHPulsePin = 6;

void setup()
{
    pinMode(g_DisplayPin,OUTPUT);
    pinMode(g_HLPulsePin,OUTPUT);
    pinMode(g_LHPulsePin,OUTPUT);

    digitalWrite(g_DisplayPin,LOW);
    digitalWrite(g_HLPulsePin,LOW);
    digitalWrite(g_LHPulsePin,HIGH);

    pinMode(g_InputPin,INPUT_PULLUP);

     Serial.begin(115200);
}

int g_counter = 0;
void loop()
{
    if( digitalRead(g_InputPin) == 0 ) {
        Serial.println(String("trigger ") + g_counter++);
        
        digitalWrite(g_HLPulsePin,HIGH);
        digitalWrite(g_LHPulsePin,LOW);
        delay(200);

        digitalWrite(g_HLPulsePin,LOW);
        digitalWrite(g_LHPulsePin,HIGH);
        delay(200);

    }
}