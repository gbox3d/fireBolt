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

#ifdef ESP32
#define LED_BUILTIN 4
#endif

Task task_Cmd(100, TASK_FOREVER, []()
              {

#ifdef MEGA2560

    if(Serial1.available() > 0) {
        String _strLine = Serial1.readStringUntil('\n');
        _strLine.trim();
        Serial1.println(_strLine);
    }
#endif

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
                _res_doc["app"] = "example-00 cli";
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
#ifdef ESP8266
                ESP.reset();
#elif ESP32
                ESP.restart();
#endif
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
                                // JsonArray array = tempDoc[key].as<JsonArray>();

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

    g_config.load();

    Serial.println(":-]");
    Serial.println("Serial connected");

#ifdef MEGA2560
    Serial1.begin(115200);
    Serial1.println(":-]");
    Serial1.println("Serial1 connected");
#endif
    
    g_ts.startNow();
}

// the loop function runs over and over again forever
void loop()
{
  g_ts.execute();
}