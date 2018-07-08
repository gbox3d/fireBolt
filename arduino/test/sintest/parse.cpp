#include <Arduino.h>
#include <ArduinoJson.h>

#include "parser.h"

String result_cmd;
String result_p1_str;
unsigned long result_p1_number;

int parseCmd(const char *_szBuf)
{

    if (_szBuf == "")
        return OP_NODATA;
    StaticJsonBuffer<128> jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(_szBuf);
    
    if (root.success())
    {
        String cmd = root["c"];
        if (cmd == "sin")
        {
            result_cmd = cmd;
            result_p1_number = root["p1"];
            return OP_SIN;
        }
        else if (cmd == "cos")
        {
            result_cmd = cmd;
            result_p1_number = root["p1"];
            return OP_COS;
        }
    }
    else
    {
        Serial.println("{\"r\":\"err\"}");
    }

    //return nOperation;
    return OP_NONE;
}


int checkUartCmd()
{
    static char szbufJson[128];
    static int nBufIndex = 0;

    while (Serial.available() > 0)
    {
        char x = Serial.read();
        if (x == '}')
        {
            szbufJson[nBufIndex++] = x;
            szbufJson[nBufIndex] = 0x00;
            nBufIndex = 0;
            return parseCmd(szbufJson);
        }
        else
        {
            szbufJson[nBufIndex++] = x;
        }
    }

    return OP_NODATA;
}

