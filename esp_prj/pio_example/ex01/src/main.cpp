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

void  ledOffCallback();
void  ledOnCallback();
Task tLedBlinker (500, TASK_FOREVER, &ledOnCallback, &g_ts, false);

void  ledOnCallback() 
{
    digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
    tLedBlinker.setCallback(&ledOffCallback);
}

void  ledOffCallback()
{ 
    digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
    tLedBlinker.setCallback(&ledOnCallback);
}

Task task_Cmd(100, TASK_FOREVER, []() {
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
                //esp8266 chip id
                _res_doc["chipid"] = ESP.getChipId();
            }
            else if(cmd == "reboot") {
                ESP.restart();
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
            else if (cmd == "blink")  
            {
              /* code */
              tLedBlinker.enable();
              _res_doc["result"] = "ok";
              _res_doc["ms"] = "led blink";
            }
            else if (cmd == "stopblk")
            {
              /* code */
              tLedBlinker.disable();
              _res_doc["result"] = "ok";
              _res_doc["ms"] = "led stop blink";
            }
            
            else {
              _res_doc["result"] = "fail";
              _res_doc["ms"] = "unknown command";
            }
            serializeJson(_res_doc, Serial);
            Serial.println();
            
        }
    }
}, &g_ts, true);


// the setup function runs once when you press reset or power the board
void setup()
{
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); // turn the LED off by making the voltage LOW

    Serial.begin(115200);

    g_config.load();

    delay(500);

    JsonDocument _doc_ledpins; 
    g_config.getArray("ledpin",_doc_ledpins);

    JsonArray ledpin = _doc_ledpins.as<JsonArray>();

    for(JsonVariant v : ledpin) {
        
        int pin = v.as<int>();
        
        pinMode(pin, OUTPUT);
        digitalWrite(pin, HIGH); // turn the LED off by making the voltage LOW
        ledPins.push_back(pin);

        Serial.print("led pin : " + String(pin) + "\n");
    }

    
    // EEPROM.begin(512);

    // while (!Serial); // wait for serial attach
    

    Serial.println(":-]");
    Serial.println("Serial connected");
#ifdef ESP8266
    Serial.println("ESP8266");
#endif
    
    g_ts.startNow();
}

// the loop function runs over and over again forever
void loop()
{
    g_ts.execute();
}