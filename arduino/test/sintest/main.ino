#include <Arduino.h>

#include "parser.h"

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
}

unsigned long pluseLength = 120000; //120 ms

void loop()
{
    // put your main code here, to run repeatedly:
    static unsigned long _curTick = micros();
    static unsigned long _prevTick = micros();
    static unsigned long _accTick = 0;
    static unsigned long _Fsm = 0;

    _curTick = micros();
    unsigned long _deltaTick = _curTick - _prevTick;
    if (_curTick < _prevTick)
    {
        _deltaTick = 0;
    }
    _prevTick = _curTick;
    _accTick += _deltaTick;

    //{c:"sin",p1:120000}
    int nResult = checkUartCmd();

    switch (_Fsm)
    {
    case 0:
    {
        if (nResult == OP_COS)
        {
            pluseLength = result_p1_number;
            _accTick = 0;
            _Fsm = 20;
        }
        else if (nResult == OP_SIN)
        {
            pluseLength = result_p1_number;
            _accTick = 0;
            _Fsm = 30;
        }
    }
    break;
    case 20:
    {

        int val = 255 * cos((3.14 / 2) * ((float)_accTick / (float)pluseLength));
        //int val = 255;
        Serial.print(val);
        Serial.print(",");
        Serial.println(_accTick);

        //delay(50);

        if (_accTick >= pluseLength)
        {
            _Fsm = 10;
        }
    }
    break;
    case 30:
    {

        int val = 255 * sin((3.14 / 2) * ((float)_accTick / (float)pluseLength));
        //int val = 255;
        Serial.print(val);
        Serial.print(",");
        Serial.println(_accTick);

        //delay(50);

        if (_accTick >= pluseLength)
        {
            _Fsm = 10;
        }
    }
    break;
    case 10:
        break;
    }
}
