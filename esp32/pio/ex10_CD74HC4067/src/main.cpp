#include <Arduino.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif ESP32
#include <WiFi.h>
#include <vector>
#endif

#include <TaskScheduler.h>
#include <ArduinoJson.h>

#include "tonkey.hpp"

#include "config.hpp"

Scheduler g_ts;
tonkey g_MainParser;

Config g_config;

std::vector<int> ledPins;


void muxout(int select, int out)
{
    for (int i = 0; i < 4; i++)
    {
        //shift out
        digitalWrite(ledPins[i], (select & 0x01) ? HIGH : LOW);
        select >>= 1;

        // Serial.println("muxout : " + String(i) + " : " + String((select & 0x01) ? HIGH : LOW));
    }

    digitalWrite(ledPins[4], out ? HIGH : LOW);
}

Task task_Cmd(100, TASK_FOREVER, []()
              {
    if (Serial.available() > 0)
    {
        String _strLine = Serial.readStringUntil('\n');
        _strLine.trim();
        Serial.println(_strLine);
        g_MainParser.parse(_strLine);

        if(g_MainParser.getTokenCount() > 0) {
          String cmd = g_MainParser.getToken(0);
            JsonDocument _res_doc;
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
            else if(cmd == "reboot") {
                ESP.restart();
            }
            else if(cmd == "muxout") { // mux 0 1

                if(g_MainParser.getTokenCount() == 3) {

                    int select = g_MainParser.getToken(1).toInt();
                    int out = g_MainParser.getToken(2).toInt();

                    muxout(select, out);

                    _res_doc["result"] = "ok";
                    _res_doc["ms"] = "mux out";

                }
                else {
                    _res_doc["result"] = "fail";
                    _res_doc["ms"] = "need select and out";
                }
            }
            else if(cmd == "config") {
                if(g_MainParser.getTokenCount() > 1) {
                    String subCmd = g_MainParser.getToken(1);
                    if(subCmd == "load") {
                        g_config.load();
                        _res_doc["result"] = "ok";
                        _res_doc["ms"] = "config loaded";
                    }
                    else if(subCmd == "save") {
                        g_config.save();
                        _res_doc["result"] = "ok";
                        _res_doc["ms"] = "config saved";
                    }
                    else if(subCmd == "dump") {
                        
                        //parse json g_config.dump()
                        String jsonStr = g_config.dump();
                        DeserializationError error = deserializeJson(_res_doc["ms"], jsonStr);
                        if (error) {
                            // Serial.print(F("deserializeJson() failed: "));
                            // Serial.println(error.f_str());
                            // return;
                            _res_doc["result"] = "fail";
                            _res_doc["ms"] = "json parse error";
                        }
                        else {
                            _res_doc["result"] = "ok";
                        }
                        
                    }
                    else if(subCmd == "clear") {
                        g_config.clear();
                        _res_doc["result"] = "ok";
                        _res_doc["ms"] = "config cleared";
                    }
                    else if(subCmd == "set") {
                        if(g_MainParser.getTokenCount() > 2) {
                            String key = g_MainParser.getToken(2);
                            String value = g_MainParser.getToken(3);
                            g_config.set(key.c_str(), value);
                            _res_doc["result"] = "ok";
                            _res_doc["ms"] = "config set";
                        }
                        else {
                            _res_doc["result"] = "fail";
                            _res_doc["ms"] = "need key and value";
                        }
                    }
                    else if(subCmd == "setA") { //set json array
                        if(g_MainParser.getTokenCount() > 2) {
                            String key = g_MainParser.getToken(2);
                            String value = g_MainParser.getToken(3);
                            //parse json value
                            // JSON 문자열 파싱을 위한 임시 객체
                            JsonDocument tempDoc; // 임시 JSON 문서

                            // JSON 문자열 파싱
                            DeserializationError error = deserializeJson(tempDoc, value);
                            // DeserializationError error = deserializeJson(g_config[key.c_str()], value);
                            if (error) {
                                // Serial.print(F("deserializeJson() failed: "));
                                // Serial.println(error.f_str());
                                // return;
                                _res_doc["result"] = "fail";
                                _res_doc["ms"] = "json parse error";
                            }
                            else {
                                JsonArray array = tempDoc[key].as<JsonArray>();

                                g_config.set(key.c_str(), tempDoc);
                                _res_doc["result"] = "ok";
                                _res_doc["ms"] = tempDoc;
                            }
                            // g_config.set(key.c_str(), value);
                            // _res_doc["result"] = "ok";
                            // _res_doc["ms"] = "config set";
                        }
                        else {
                            _res_doc["result"] = "fail";
                            _res_doc["ms"] = "need key and value";
                        }
                        
                    }
                    else if(subCmd == "get") {
                        if(g_MainParser.getTokenCount() > 2) {
                            String key = g_MainParser.getToken(2);

                            //check key exist
                            if(!g_config.hasKey(key.c_str())) {
                                _res_doc["result"] = "fail";
                                _res_doc["ms"] = "key not exist";
                                // serializeJson(_res_doc, Serial);
                                // Serial.println();
                                // return;
                            }
                            else {
                                _res_doc["result"] = "ok";
                                _res_doc["value"] = g_config.get<String>(key.c_str());
                            }
                        }
                        else {
                            _res_doc["result"] = "fail";
                            _res_doc["ms"] = "need key";
                        }
                    }
                    else {
                        _res_doc["result"] = "fail";
                        _res_doc["ms"] = "unknown sub command";
                    
                    }
                }
                else {
                    _res_doc["result"] = "fail";
                    _res_doc["ms"] = "need sub command";
                }
            }
            else if (cmd == "on")
            {
              /* code */

              if( g_MainParser.getTokenCount() > 1) {
                int pinIndex = g_MainParser.getToken(1).toInt();

                int pin = ledPins[pinIndex];

                digitalWrite(pin, HIGH); // turn the LED on (HIGH is the voltage level)

                _res_doc["result"] = "ok";
                _res_doc["ms"] = "led on";
              }
              else {
                digitalWrite(LED_BUILTIN, LOW); // turn the LED on (HIGH is the voltage level)
                _res_doc["result"] = "ok";
                _res_doc["ms"] = "led on";
              }


              
            }
            else if (cmd == "off")
            {
                if( g_MainParser.getTokenCount() > 1) {
                int pinIndex = g_MainParser.getToken(1).toInt();

                int pin = ledPins[pinIndex];

                digitalWrite(pin, LOW); // turn the LED on (HIGH is the voltage level)

                _res_doc["result"] = "ok";
                _res_doc["ms"] = "led on";
              }
              else {
                digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
                _res_doc["result"] = "ok";
                _res_doc["ms"] = "led on";
              }
            }
            
            else {
              _res_doc["result"] = "fail";
              _res_doc["ms"] = "unknown command";
            }
            serializeJson(_res_doc, Serial);
            Serial.println();
            
        }
    } }, &g_ts, true);

// the setup function runs once when you press reset or power the board
void setup()
{
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); // turn the LED off by making the voltage LOW

    Serial.begin(115200);

    delay(500);
    
    //config setA ledpin [25,26,27,14,13]
    JsonDocument _doc_ledpins;
    g_config.getArray("ledpin", _doc_ledpins);

    JsonArray ledpin = _doc_ledpins.as<JsonArray>();

    for (JsonVariant v : ledpin)
    {
        int pin = v.as<int>();
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW); // turn the LED off by making the voltage LOW
        // digitalWrite(pin, HIGH); // turn the LED off by making the voltage LOW
        ledPins.push_back(pin);
        Serial.print("led pin : " + String(pin) + "\n");
    }

    Serial.println(":-]");
    Serial.println("Serial connected");
    
    g_config.dump();

#ifdef ESP8266
    Serial.println("ESP8266");
#endif

    g_ts.startNow();
}

// the loop function runs over and over again forever
void loop()
{

    // muxout(0, 1);
    // muxout(1, 1);


    g_ts.execute();
}