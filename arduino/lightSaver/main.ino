#include <Arduino.h>

#include <ArduinoJson.h>

extern volatile unsigned long timer0_millis; //전역 타이머변수

#define DIGIT_NUM 3
int pinMapLed[8] = {7, 8, 12, 13, 14, 5, 6, 8};
int pinMapSelector[DIGIT_NUM] = {9, 10, 15};

const int g_OnOffBtnPort = 2;
const int g_LightControlPort = 12;

int g_nFsm = 0;
int g_nNextFsm = 0;
unsigned long g_timeOverValue; // 1시간..

void setup()
{

    Serial.begin(115200);

    pinMode(g_OnOffBtnPort,INPUT_PULLUP);
    pinMode(g_LightControlPort,OUTPUT);

    g_timeOverValue = 1000l * 60l * 60L * 1L;

    Serial.println("{\"version\":100,\"di\":0x40}");
    Serial.println(g_timeOverValue);
}


//const long g_timeOverValue = 1000*5;

unsigned long _prevTick1 = 0;
unsigned long _workTick1 = 0;
void loop()
{

    // put your main code here, to run repeatedly:
    //Serial.println(digitalRead(g_OnOffBtnPort));

    switch (g_nFsm)
    {
    case 0: //off
    {
        Serial.println("Off");
        digitalWrite(g_LightControlPort, LOW);
        g_nFsm = 1;
    }   
    break;
    case 1:
    {
        if (digitalRead(g_OnOffBtnPort) == 0)
        {
            g_nFsm = 100;
            g_nNextFsm = 10;
            //Serial.println("On");
            
        }

    }
    break;
    case 10: //on
    {
        Serial.println("On");
        digitalWrite(g_LightControlPort, HIGH);
        timer0_millis = 0;
        g_nFsm = 11;
        _prevTick1 = 0;
        _workTick1 = 0;

    }
    break;
    case 11:
    {
        //digitalWrite(pinMapLed[0], LOW);
        if (digitalRead(g_OnOffBtnPort) == 0)
        {
            g_nFsm = 100;
            g_nNextFsm = 0;
            //Serial.println("Off");
        }
        else {
            if( (millis() - _workTick1) > 1000l ) {
                //Serial.println(_workTick1);
                //Serial.println(millis());
                //Serial.println(g_timeOverValue);
                Serial.println("{\"cur\":" + String(millis()) + "}");
                _workTick1 = millis();
            }
            //_workTick1 += (millis() - _prevTick1);
            if (millis() >= g_timeOverValue) {
                Serial.println("timeover now Off");
                g_nFsm = 0;
            }

           //_prevTick1 = millis()

        }
    }
    break;
    case 100:
    {
        timer0_millis = 0;
        g_nFsm = 101;
    }
    break;
    case 101:
    {
        if (millis() >= 300)
        {
            timer0_millis = 0;
            g_nFsm = g_nNextFsm;
        }
    }
    break;
    }
}
