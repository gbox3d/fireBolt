#include <Arduino.h>

String _strLine;

//함수 포인터를 인자로 받는다.
bool processSerialCmder(void (*_doProcess)(String))
{
    if (Serial.available() > 0)
    {
        // 하나씩 읽어서 버퍼에 쌓는다.
        char _ch = Serial.read();
        if (_ch == '\n')
        {
            _strLine.trim();
            _doProcess(_strLine);
            _strLine = "";
        }
        else
        {
            _strLine += _ch;
        }
    }
    return true;
}

