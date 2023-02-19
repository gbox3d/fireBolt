#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>

#include <TaskScheduler.h>
#include <ArduinoJson.h>

#include "tonkey.hpp"

//EEPROM 예제
// esp-Wroom 의 경우는 보드 매니져 에서 D1 mini lite-> flash size 1M(FS:none) 로 설정해야 한다.

// const char *ssid = "your_ssid";
// const char *password = "your_password";

const u32 CHECKCODE = 20230218;

struct S_CONFIG_DATA {
    u32 checkcode;
    char ssid[16];
    char passwd[16];
    char remote_ip[32];
    int remote_port;
};

void WiFiEvent(WiFiEvent_t event)
{
    switch (event)
    {
    case WIFI_EVENT_STAMODE_CONNECTED:
        Serial.println("WiFi connected");

        break;
    case WIFI_EVENT_STAMODE_AUTHMODE_CHANGE:
        Serial.println("Authentication mode of access point has changed");
        break;
    case WIFI_EVENT_STAMODE_GOT_IP:
        Serial.println("Get IP address");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        digitalWrite(LED_BUILTIN, LOW);
        break;
    case WIFI_EVENT_STAMODE_DISCONNECTED:
        Serial.println("WiFi lost connection");
        digitalWrite(LED_BUILTIN, HIGH);
        break;
    case WIFI_EVENT_STAMODE_DHCP_TIMEOUT:
        Serial.println("DHCP timeout");
        break;
    case WIFI_EVENT_SOFTAPMODE_STACONNECTED:
        Serial.println("Station connected to soft-AP");
        break;
    case WIFI_EVENT_SOFTAPMODE_STADISCONNECTED:
        Serial.println("Station disconnected from soft-AP");
        break;
    case WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED:
        Serial.println("Probe request received");
        break;
    default:
        Serial.printf("[WiFi-event] event: %d\n", event);
        break;
    }
}

Scheduler runner;
tonkey g_MainParser;
S_CONFIG_DATA g_config_data;

void _cmdparser() {
    if (Serial.available() > 0)
    {
        String _strLine = Serial.readStringUntil('\n');
        _strLine.trim();

        g_MainParser.parse(_strLine);

        if(g_MainParser.getTokenCount() > 0)
        {
            String cmd = g_MainParser.getToken(0);
            DynamicJsonDocument doc(1024);

            if(cmd == "put") {
                
                // EEPROM.write(0,'h');
                EEPROM.put(0,"hello");
                delay(500);
                EEPROM.commit();
                Serial.println("put ok");
                // EEPROM.end();
            }
            else if (cmd == "commit") {
                EEPROM.commit();
                Serial.println("commit");
            }
            else if (cmd == "get")
            {
                char _str[10];
                EEPROM.get(0,_str);
                delay(500);
                Serial.println(_str);
                // Serial.println("get ok");
                // // EEPROM.end();
                /* code */
            }
            else if (cmd == "reboot")
            {
                /* code */
                doc["result"] = "ok";
                doc["msg"] = "reboot";

                Task *t = new Task(500, TASK_ONCE, []() {
                    ESP.restart();
                },&runner, true);

                // ESP.restart();
            }
            else if (cmd == "wifi")
            {
                /* code */
                String ssid = g_MainParser.getToken(1);
                String password = g_MainParser.getToken(2);

                strcpy(g_config_data.ssid, ssid.c_str());
                strcpy(g_config_data.passwd, password.c_str());

                WiFi.begin(ssid.c_str(), password.c_str());

                doc["result"] = "ok";
                doc["msg"] = "wifi connect";


                // Serial.println("ok : wifi connect");
            }
            else if (cmd == "remote")
            {
                /* code */
                String ip = g_MainParser.getToken(1);
                String port = g_MainParser.getToken(2);
                
                strcpy(g_config_data.remote_ip, ip.c_str());
                g_config_data.remote_port = port.toInt();

                // Serial.printf("ok : set remote ip -> %s, port -> %d \n", g_config_data.remote_ip, g_config_data.remote_port);

                doc["result"] = "ok";
                doc["msg"]= "set remote";


                // strcpy(g_config_data.remote_port, port.c_str());
            }
            else if (cmd == "save")
            {
                /* code */

                g_config_data.checkcode = CHECKCODE;

                EEPROM.put(0, g_config_data);
                EEPROM.commit();

                doc["result"] = "ok";
                doc["msg"] = "save config";

                // Serial.println("ok : save config");
            }
            else if (cmd=="load")
            {
                /* code */
                EEPROM.get(0, g_config_data);

                if(g_config_data.checkcode != CHECKCODE) {
                    doc["result"] = "fail";
                    doc["msg"] = "not found config";
                    // return;
                }
                else {
                    doc["result"] = "ok";
                    doc["ssid"] = g_config_data.ssid;
                    doc["passwd"] = g_config_data.passwd;
                    doc["remote_ip"] = g_config_data.remote_ip;
                    doc["remote_port"] = g_config_data.remote_port;
                }
                
                // Serial.println("ok : load config");
                // Serial.printf("ssid : %s ,passwd : %s, ip : %s, port : %d \n", g_config_data.ssid, g_config_data.passwd, g_config_data.remote_ip, g_config_data.remote_port);
            }
            
            else if (cmd == "reconnect")
            {
                /* code */
                EEPROM.get(0, g_config_data);
                delay(500);
                WiFi.begin(g_config_data.ssid, g_config_data.passwd);
                // Serial.println("reconnect");
                doc["result"] = "ok";
                doc["ssid"] = g_config_data.ssid;
                doc["passwd"] = g_config_data.passwd;
            }
            else if (cmd == "disconnect")
            {
                /* code */
                WiFi.disconnect();
                doc["result"] = "ok";
                doc["msg"] = "disconnect";
            }
            else if (cmd == "about")
            {
                /* code */
                // Serial.println("exam 08");
                
                doc["result"] = "ok";
                doc["name"] = "exam 08";
                doc["version"] = "1.0.0";
            }
            else {
                doc["result"] = "fail";
                doc["msg"] = "unknown command";
            }
            serializeJson(doc, Serial);
            
        }
    }


}

Task main_task(100, TASK_FOREVER, []() {
    _cmdparser();
},&runner,true);


void setup()
{
    Serial.begin(115200);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    WiFi.onEvent(WiFiEvent);
    while (!Serial);
    
    EEPROM.begin(sizeof(S_CONFIG_DATA));
    Serial.println("\n\n");
    Serial.println("Ready example 8 : Task , EEPROM, Json, WiFi config");

    runner.startNow();  // set point-in-time for scheduling start
}


void loop()
{
    runner.execute();
}
