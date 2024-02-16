#include <Arduino.h> // Arduino 헤더 파일 포함
#include <TaskScheduler.h>
#include <ArduinoJson.h>
#include <tonkey.hpp>

#include "config.hpp"

#ifdef SEED_XIAO_ESP32C3
#define LED_PIN D0
#elif defined(LOLIN_D32)
#define LED_PIN 4
#endif


Scheduler g_ts;
tonkey g_MainParser;
CCongifData configData;


Task task_Cmd(
    100, TASK_FOREVER, []()
    {
    if (Serial.available() > 0)
    {
        String _strLine = Serial.readStringUntil('\n');
        _strLine.trim();
        Serial.println(_strLine);
        g_MainParser.parse(_strLine);

        if(g_MainParser.getTokenCount() > 0) {
          String cmd = g_MainParser.getToken(0);
            DynamicJsonDocument _res_doc(1024);

            if (cmd == "about")
            {
                /* code */
                _res_doc["result"] = "ok";
                _res_doc["title"] = "example 01 - hello esp32";
                _res_doc["version"] = "1.0.0";
                _res_doc["author"] = "gbox3d";
            }
            else if(cmd == "save") {
              configData.save();
              _res_doc["result"] = "ok";
            }
            else if ((cmd == "load")) 
            {
              configData.load();
              _res_doc["result"] = "ok";
            }
            else if ((cmd == "clear")) 
            {
              configData.clear();
              _res_doc["result"] = "ok";
            }
            else if ((cmd == "print")) 
            {
              configData.print();
              _res_doc["result"] = "ok";
            }
            else if (cmd == "set") {
              String key = g_MainParser.getToken(1);
              String value = g_MainParser.getToken(2);
              
              _res_doc["result"] = "ok";

              if(key == "ap") {
                configData.mStrAp = value;
              }
              else if(key == "password") {
                configData.mStrPassword = value;
              }
              else if(key == "target_ip") {
                configData.mTargetIp = value;
              }
              else if(key == "target_port") {
                configData.mTargetPort = value.toInt();
              }
              else {
                _res_doc["result"] = "fail";
                _res_doc["ms"] = "unknown key";
              }
            }
            else if (cmd == "get") {
              String key = g_MainParser.getToken(1);
              if(key == "ap") {
                _res_doc["value"] = configData.mStrAp;
              }
              else if(key == "password") {
                _res_doc["value"] = configData.mStrPassword;
              }
              else if(key == "target_ip") {
                _res_doc["value"] = configData.mTargetIp;
              }
              else if(key == "target_port") {
                _res_doc["value"] = configData.mTargetPort;
              }
              else if(key == "offset") {
                int index = g_MainParser.getToken(2).toInt();
                _res_doc["value"] = configData.mOffsets[index];
              }
              else {
                _res_doc["result"] = "fail";
                _res_doc["ms"] = "unknown key";
              }
              _res_doc["result"] = "ok";

            }
            else if(cmd=="led") {
              
              String value = g_MainParser.getToken(1);
              _res_doc["result"] = "ok";

              if(value == "on") {
                digitalWrite(LED_PIN, HIGH);
              }
              else if(value == "off") {
                digitalWrite(LED_PIN, LOW);
              }
              else if(value == "toggle") {
                digitalWrite(LED_PIN, !digitalRead(LED_PIN));
              }
              else {
                _res_doc["result"] = "fail";
                _res_doc["ms"] = "unknown value";
              }
            }
            else if (cmd == "add") {
              int a = g_MainParser.getToken(1).toInt();
              int b = g_MainParser.getToken(2).toInt();
              _res_doc["result"] = "ok";
              _res_doc["value"] = a + b;
            }
            else if( cmd == "reboot") {
              ESP.restart();
            }
            else {
              _res_doc["result"] = "fail";
              _res_doc["ms"] = "unknown command";
            }

            serializeJson(_res_doc, Serial);
            Serial.println();
        }
    } },
    &g_ts, true);

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  Serial.begin(115200); // 시리얼 통신 초기화

  // while (!Serial)
  //   ; // wait for serial attach
  // delay(500);

  Serial.println(":-]");
  Serial.println("Serial connected");

  g_ts.startNow();
}

void loop()
{
  g_ts.execute();
}