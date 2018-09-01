#include <Arduino.h>

extern volatile unsigned long timer0_millis; //전역 타이머변수

#define DIGIT_NUM 3
int pinMapLed[8] = {7, 8, 12, 13, 14, 5, 6, 8};
int pinMapSelector[DIGIT_NUM] = {9, 10, 15};

const int g_OnOffBtnPort = 2;
const int g_LightControlPort = 12;

void setup()
{

    Serial.begin(115200);

    pinMode(g_OnOffBtnPort,INPUT_PULLUP);
    pinMode(g_LightControlPort,OUTPUT);

    Serial.println("{\"version\":100,\"di\":0x40}");
}

int g_nFsm = 0;
int g_nNextFsm = 0;
const long g_timeOverValue = 1000*60*60 * 1; // 1시간..
//const long g_timeOverValue = 1000*5;

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
        g_nFsm = 11;

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
            if (millis() >= g_timeOverValue) {
                Serial.println("timeover now Off");
                g_nFsm = 0;

            }

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
