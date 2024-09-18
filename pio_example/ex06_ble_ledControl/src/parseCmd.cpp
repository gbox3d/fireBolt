#include <Arduino.h>

#include "tonkey.hpp"

#include "config.hpp"

tonkey g_MainParser;

extern Config g_config;

extern void ledOn(int pinIndex);
extern void ledOff(int pinIndex);

extern void startBlink();
extern void stopBlink();

String parseCmd(String _strLine)
{

    JsonDocument _res_doc;

    g_MainParser.parse(_strLine);

    if (g_MainParser.getTokenCount() > 0)
    {
        String cmd = g_MainParser.getToken(0);
        // JsonDocument _res_doc;

        if (cmd == "about")
        {
            /* code */
            _res_doc["result"] = "ok";
            _res_doc["os"] = "cronos-v1";
            _res_doc["app"] = "example 01 - led";
            _res_doc["version"] = "1.0.0";
            _res_doc["author"] = "gbox3d";
// esp8266 chip id
#ifdef ESP8266
            _res_doc["chipid"] = ESP.getChipId();
#elif ESP32
            _res_doc["chipid"] = ESP.getEfuseMac();
#endif
        }
        else if (cmd == "reboot")
        {
            ESP.restart();
        }
        else if (cmd == "config")
        {
            if (g_MainParser.getTokenCount() > 1)
            {
                String subCmd = g_MainParser.getToken(1);
                if (subCmd == "load")
                {
                    g_config.load();
                    _res_doc["result"] = "ok";
                    _res_doc["ms"] = "config loaded";
                }
                else if (subCmd == "save")
                {
                    g_config.save();
                    _res_doc["result"] = "ok";
                    _res_doc["ms"] = "config saved";
                }
                else if (subCmd == "dump")
                {

                    // parse json g_config.dump()
                    String jsonStr = g_config.dump();
                    DeserializationError error = deserializeJson(_res_doc["ms"], jsonStr);
                    if (error)
                    {
                        // Serial.print(F("deserializeJson() failed: "));
                        // Serial.println(error.f_str());
                        // return;
                        _res_doc["result"] = "fail";
                        _res_doc["ms"] = "json parse error";
                    }
                    else
                    {
                        _res_doc["result"] = "ok";
                    }
                }
                else if (subCmd == "clear")
                {
                    g_config.clear();
                    _res_doc["result"] = "ok";
                    _res_doc["ms"] = "config cleared";
                }
                else if (subCmd == "set")
                {
                    if (g_MainParser.getTokenCount() > 2)
                    {
                        String key = g_MainParser.getToken(2);
                        String value = g_MainParser.getToken(3);
                        g_config.set(key.c_str(), value);
                        _res_doc["result"] = "ok";
                        _res_doc["ms"] = "config set";
                    }
                    else
                    {
                        _res_doc["result"] = "fail";
                        _res_doc["ms"] = "need key and value";
                    }
                }
                else if (subCmd == "setA")
                { // set json array
                    if (g_MainParser.getTokenCount() > 2)
                    {
                        String key = g_MainParser.getToken(2);
                        String value = g_MainParser.getToken(3);
                        // parse json value
                        //  JSON 문자열 파싱을 위한 임시 객체
                        JsonDocument tempDoc; // 임시 JSON 문서

                        // JSON 문자열 파싱
                        DeserializationError error = deserializeJson(tempDoc, value);
                        // DeserializationError error = deserializeJson(g_config[key.c_str()], value);
                        if (error)
                        {
                            // Serial.print(F("deserializeJson() failed: "));
                            // Serial.println(error.f_str());
                            // return;
                            _res_doc["result"] = "fail";
                            _res_doc["ms"] = "json parse error";
                        }
                        else
                        {
                            JsonArray array = tempDoc[key].as<JsonArray>();

                            g_config.set(key.c_str(), tempDoc);
                            _res_doc["result"] = "ok";
                            _res_doc["ms"] = tempDoc;
                        }
                        // g_config.set(key.c_str(), value);
                        // _res_doc["result"] = "ok";
                        // _res_doc["ms"] = "config set";
                    }
                    else
                    {
                        _res_doc["result"] = "fail";
                        _res_doc["ms"] = "need key and value";
                    }
                }
                else if (subCmd == "get")
                {
                    if (g_MainParser.getTokenCount() > 2)
                    {
                        String key = g_MainParser.getToken(2);

                        // check key exist
                        if (!g_config.hasKey(key.c_str()))
                        {
                            _res_doc["result"] = "fail";
                            _res_doc["ms"] = "key not exist";
                            // serializeJson(_res_doc, Serial);
                            // Serial.println();
                            // return;
                        }
                        else
                        {
                            _res_doc["result"] = "ok";
                            _res_doc["value"] = g_config.get<String>(key.c_str());
                        }
                    }
                    else
                    {
                        _res_doc["result"] = "fail";
                        _res_doc["ms"] = "need key";
                    }
                }
                else
                {
                    _res_doc["result"] = "fail";
                    _res_doc["ms"] = "unknown sub command";
                }
            }
            else
            {
                _res_doc["result"] = "fail";
                _res_doc["ms"] = "need sub command";
            }
        }
        else if (cmd == "on")
        {
            /* code */
            int tokenCount = g_MainParser.getTokenCount(); // 토큰 개수를 미리 변수에 저장
            if (tokenCount > 1)
            {
                for (int i = 1; i < tokenCount; i++) // tokenCount를 사용
                {
                    int pinIndex = g_MainParser.getToken(i).toInt();
                    ledOn(pinIndex);
                }

                _res_doc["result"] = "ok";
                _res_doc["ms"] = "led(s) on";
            }
            else
            {
                _res_doc["result"] = "err";
                _res_doc["ms"] = "need pin index";
            }
        }
        else if (cmd == "off")
        {
            int tokenCount = g_MainParser.getTokenCount(); // 동일하게 tokenCount를 미리 저장
            if (tokenCount > 1)
            {
                for (int i = 1; i < tokenCount; i++) // tokenCount를 사용
                {
                    int pinIndex = g_MainParser.getToken(i).toInt();
                    ledOff(pinIndex);
                }

                _res_doc["result"] = "ok";
                _res_doc["ms"] = "led(s) off";
            }
            else
            {
                _res_doc["result"] = "err";
                _res_doc["ms"] = "need pin index";
            }
        }

        else if (cmd == "blink")
        {
            /* code */
            startBlink();
            _res_doc["result"] = "ok";
            _res_doc["ms"] = "led blink";
        }
        else if (cmd == "stopblk")
        {
            /* code */
            stopBlink();
            _res_doc["result"] = "ok";
            _res_doc["ms"] = "led stop blink";
        }
        else
        {
            _res_doc["result"] = "fail";
            _res_doc["ms"] = "unknown command";
        }
    }
    else
    {
        _res_doc["result"] = "fail";
        _res_doc["ms"] = "need command";
    }

    return _res_doc.as<String>();
}