#include <Arduino.h> // Arduino 헤더 파일 포함
#include <WiFi.h>
#include <AsyncUDP.h>

#include <TaskScheduler.h>
#include <ArduinoJson.h>
#include <tonkey.hpp>



#include "config.hpp"

#ifdef SEED_XIAO_ESP32C3
#define LED_PIN D0
#define ANALOG_PIN A0
#elif defined(LOLIN_D32)
#define LED_PIN 4
#elif defined(BEETLE_ESP32C3)
#define LED_PIN 10
#define ANALOG_PIN A0
#define BTN_PIN 7
#elif defined(WROVER_KIT)
#define LED_PIN 5
#define ANALOG_PIN 34
#endif


Scheduler g_runner;
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
            else if(cmd == "digital") {
              String value = g_MainParser.getToken(1);
              _res_doc["result"] = "ok";
              _res_doc["value"] = digitalRead(BTN_PIN);

              
            }
            else if(cmd == "analog") {
              _res_doc["result"] = "ok";
              _res_doc["value"] = analogRead(ANALOG_PIN);
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
            else if(cmd == "wifi") {
              String value = g_MainParser.getToken(1);
              if(value == "connect") {

                digitalWrite(LED_PIN, LOW);
                
                WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
                  Serial.printf("[WiFi-event] event: %d\n", event);

                  switch(event) {
                    case SYSTEM_EVENT_STA_GOT_IP:
                      Serial.println("WiFi connected");
                      digitalWrite(LED_PIN, HIGH);
                      break;
                    case SYSTEM_EVENT_STA_DISCONNECTED:
                      Serial.println("WiFi lost connection");
                      digitalWrite(LED_PIN, LOW);
                      break;
                    default: break;
                  }
                  // if(event == SYSTEM_EVENT_STA_GOT_IP) {
                  //   Serial.println("WiFi connected");
                  //   digitalWrite(LED_PIN, HIGH);

                  // }
                  // else if(event == SYSTEM_EVENT_STA_DISCONNECTED) {
                  //   Serial.println("WiFi disconnected");
                  //   digitalWrite(LED_PIN, LOW);
                  // }
                });

                WiFi.mode(WIFI_STA);
                WiFi.begin(configData.mStrAp.c_str(), configData.mStrPassword.c_str());

                _res_doc["result"] = "ok";
              }
              else if(value == "disconnect") {
                WiFi.disconnect();
                _res_doc["result"] = "ok";
              }
              else {
                _res_doc["result"] = "fail";
                _res_doc["ms"] = "unknown value";
              }
            }
            else {
              _res_doc["result"] = "fail";
              _res_doc["ms"] = "unknown command";
            }

            serializeJson(_res_doc, Serial);
            Serial.println();
        }
    } });

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  pinMode(BTN_PIN, INPUT);
  pinMode(ANALOG_PIN, INPUT);

  Serial.begin(115200); // 시리얼 통신 초기화
  
  Serial.println(":-]");
  Serial.println("Serial connected");

  // g_ts.startNow();
  g_runner.addTask(task_Cmd);
  task_Cmd.enable();
}

void loop()
{
  g_runner.execute();
}
