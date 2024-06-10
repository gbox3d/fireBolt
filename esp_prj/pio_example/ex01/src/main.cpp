#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <TaskScheduler.h>
#include <ArduinoJson.h>

#include "tonkey.hpp"

#include "config.hpp"

Scheduler g_ts;
tonkey g_MainParser;

Config g_config;

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
                _res_doc["title"] = "example 01 - led";
                _res_doc["version"] = "1.0.0";
                _res_doc["author"] = "gbox3d";
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
                        _res_doc["result"] = "ok";
                        // _res_doc["ms"] = g_config.dump();
                        Serial.println(g_config.dump());
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
                    else if(subCmd == "get") {
                        if(g_MainParser.getTokenCount() > 2) {
                            String key = g_MainParser.getToken(2);
                            _res_doc["result"] = "ok";
                            _res_doc["value"] = g_config.get<String>(key.c_str());
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
              digitalWrite(LED_BUILTIN, LOW); // turn the LED on (HIGH is the voltage level)
              _res_doc["result"] = "ok";
              _res_doc["ms"] = "led on";
            }
            else if (cmd == "off")
            {
              /* code */
              digitalWrite(LED_BUILTIN, HIGH);  // turn the LED off by making the voltage LOW
              _res_doc["result"] = "ok";
              _res_doc["ms"] = "led off";
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
    
    Serial.begin(115200);
    // EEPROM.begin(512);

    while (!Serial); // wait for serial attach
    delay(500);

    Serial.println(":-]");
    Serial.println("Serial connected");
    
    g_ts.startNow();
}

// the loop function runs over and over again forever
void loop()
{
    g_ts.execute();
}