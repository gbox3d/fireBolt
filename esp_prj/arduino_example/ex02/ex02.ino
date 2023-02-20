/*
 *  This sketch shows the WiFi event usage
 *
 *  In this example you can receive and process below events.
 *  Refer to ESP8266WiFiType.h
 *  
 typedef enum {
    WIFI_EVENT_STAMODE_CONNECTED = 0,
    WIFI_EVENT_STAMODE_DISCONNECTED,
    WIFI_EVENT_STAMODE_AUTHMODE_CHANGE,
    WIFI_EVENT_STAMODE_GOT_IP,
    WIFI_EVENT_STAMODE_DHCP_TIMEOUT,
    WIFI_EVENT_SOFTAPMODE_STACONNECTED,
    WIFI_EVENT_SOFTAPMODE_STADISCONNECTED,
    WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED,
    WIFI_EVENT_MAX
} WiFiEvent_t;
 * 
 * hardcopyworld.com
 */

#include <ESP8266WiFi.h>
#include <TaskScheduler.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

#include "tonkey.hpp"

Scheduler g_ts;
tonkey g_MainParser;

const u32 ROM_CHECKCODE = 20230219;
struct S_CONFIG_DATA {
    u32 checkcode;
    char ssid[16];
    char passwd[16];
    char remote_ip[32];
    int remote_port;
};

S_CONFIG_DATA g_config_data;

Task task_Cmd(100, TASK_FOREVER, []() {

    if (Serial.available() > 0)
    {
        String _strLine = Serial.readStringUntil('\n');
        _strLine.trim();

        g_MainParser.parse(_strLine);

        if(g_MainParser.getTokenCount() > 0)
        {
            String cmd = g_MainParser.getToken(0);
            DynamicJsonDocument _res_doc(1024);

            if (cmd == "about")
            {
                /* code */
                _res_doc["result"] = "ok";
                _res_doc["title"] = "exaple 07 - udp";
                _res_doc["version"] = "1.0.0";
                _res_doc["author"] = "gbox3d";
            }
            else if (cmd == "connect")
            {
                /* code */

                EEPROM.get(0, g_config_data);
                delay(500);

                WiFi.mode(WIFI_STA);
                WiFi.begin(g_config_data.ssid, g_config_data.passwd);

                while (WiFi.status() != WL_CONNECTED)
                {
                    delay(500);
                    Serial.print(".");
                }

                digitalWrite(LED_BUILTIN, LOW); //on

                _res_doc["result"] = "ok";
                _res_doc["msg"] = "connect ok";
                _res_doc["ip"] = WiFi.localIP().toString();
            }
            else if (cmd == "set") 
            {
                /* code */

                String ssid = g_MainParser.getToken(1);
                String passwd = g_MainParser.getToken(2);
                
                strcpy(g_config_data.ssid, ssid.c_str());
                strcpy(g_config_data.passwd, passwd.c_str());
                
                g_config_data.checkcode = ROM_CHECKCODE;

                EEPROM.put(0, g_config_data);
                delay(500);
                EEPROM.commit();

                _res_doc["result"] = "ok";
                _res_doc["msg"] = "set";
            }
            else if (cmd == "get") 
            {
                /* code */

                EEPROM.get(0, g_config_data);
                delay(500);

                _res_doc["result"] = "ok";
                _res_doc["msg"] = "get";
                _res_doc["ssid"] = g_config_data.ssid;
                _res_doc["passwd"] = g_config_data.passwd;
            }
            else {
                _res_doc["result"] = "fail";
                _res_doc["msg"] = "unknown command";
            }
            serializeJson(_res_doc, Serial);
        }

        
    }
}, &g_ts, true);

// Task task_WiFi(1000, TASK_FOREVER, []() {
//     if (WiFi.status() != WL_CONNECTED)
//     {
//             // delete old config
//         WiFi.disconnect(true);

//         delay(1000);

//         WiFi.onEvent(WiFiEvent);

//         WiFi.begin(ssid, password);

//         Serial.println();
//         Serial.println();
//         Serial.println("Wait for WiFi... ");
//     }
// }, &g_ts, true);


const char *ssid = "your_ssid";
const char *password = "your_password";

void WiFiEvent(WiFiEvent_t event) 
{
    switch(event) 
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

void setup() 
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); //off

    Serial.begin(115200);
    EEPROM.begin(512);
    while (!Serial);
    delay(500); // cool time
    Serial.println(":-]");
    Serial.println("Serial connected");

    EEPROM.get(0, g_config_data);
    delay(500);

    g_ts.startNow();
}


void loop() 
{
    g_ts.execute();
}
