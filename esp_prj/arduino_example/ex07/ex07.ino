#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncUDP.h>

#include <ArduinoJson.h>
#include <EEPROM.h>
#include <TaskScheduler.h>
#include "tonkey.hpp"

AsyncUDP udp;
const u32 ROM_CHECKCODE = 20230219;

struct S_HOTSHOT_PACKET_REQUEST
{
    u32 checkcode;
    u8 cmd;
    u8 extra[11];
};

struct S_HOTSHOT_PACKET_RESPONSE
{
    u32 checkcode;
    u8 cmd;
    u8 extra[3];
};

struct S_CONFIG_DATA {
    u32 checkcode;
    char ssid[16];
    char passwd[16];
    char remote_ip[32];
    int remote_port;
};

Scheduler g_ts;
tonkey g_MainParser;
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
                // Serial.println("exam 08");
                
                _res_doc["result"] = "ok";
                _res_doc["title"] = "exaple 07 - udp";
                _res_doc["version"] = "1.0.0";
                _res_doc["CHIPID"] = ESP.getChipId();
            }
            else if (cmd == "set")
            {
                /* code */
                String ssid = g_MainParser.getToken(1);
                String passwd = g_MainParser.getToken(2);
                String remote_ip = g_MainParser.getToken(3);
                int remote_port = g_MainParser.getToken(4).toInt();

                strcpy(g_config_data.ssid, ssid.c_str());
                strcpy(g_config_data.passwd, passwd.c_str());
                strcpy(g_config_data.remote_ip, remote_ip.c_str());
                g_config_data.remote_port = remote_port;
                
                g_config_data.checkcode = ROM_CHECKCODE;

                EEPROM.put(0, g_config_data);
                delay(500);
                EEPROM.commit();

                _res_doc["result"] = "ok";
                _res_doc["msg"] = "set ok";
            }
            else if (cmd == "get") 
            {
                /* code */
                EEPROM.get(0, g_config_data);
                delay(500);

                _res_doc["result"] = "ok";
                _res_doc["msg"] = "get ok";
            }
            else if (cmd == "reboot")
            {
                /* code */
                _res_doc["result"] = "ok";
                _res_doc["msg"] = "reboot";

                Task *t = new Task(500, TASK_ONCE, []() {
                    ESP.restart();
                }, &g_ts, true);
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

                _res_doc["result"] = "ok";
                _res_doc["msg"] = "connect ok";
                _res_doc["ip"] = WiFi.localIP().toString();
            }
            else if (cmd == "ping")
            {
                /* code */
                S_HOTSHOT_PACKET_REQUEST packet;
                packet.checkcode = 20230216;
                packet.cmd = 0x10;
                AsyncUDPMessage udpMessage;
                udpMessage.write((uint8_t*)&packet, sizeof(packet));
                
                IPAddress remoteIp;
                WiFi.hostByName(g_config_data.remote_ip, remoteIp); //문자형 주소를 IP로 변환
                // remoteIp.fromString(remoteIp);
                
                udp.sendTo(udpMessage,
                        remoteIp,
                        g_config_data.remote_port);
                
                _res_doc["result"] = "ok";
                _res_doc["msg"] = "send ping ok";
            }
            
            else {
                _res_doc["result"] = "fail";
                _res_doc["msg"] = "unknown command";
            }

            serializeJson(_res_doc, Serial);

        }
    }

}, &g_ts, true);

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    EEPROM.begin(512);
    while (!Serial);
    delay(500); // cool time
    Serial.println(":-]");
    Serial.println("Serial connected");

    EEPROM.get(0, g_config_data);
    delay(500);

    if(g_config_data.checkcode != ROM_CHECKCODE) {
        Serial.println("EEPROM data is not valid");
    }
    else {
        Serial.println("EEPROM data is valid");
    }

    // start udp service
    if(udp.listen(8060)) {
        Serial.print("UDP Listening ok");
        // Serial.println(WiFi.localIP());
        udp.onPacket([](AsyncUDPPacket packet) {
                Serial.println();
                Serial.print("UDP Packet Type: ");
                Serial.print(packet.isBroadcast()?"Broadcast":packet.isMulticast()?"Multicast":"Unicast");
                Serial.print(", From: ");
                Serial.print(packet.remoteIP());
                Serial.print(":");
                Serial.print(packet.remotePort());
                Serial.print(", To: ");
                Serial.print(packet.localIP());
                Serial.print(":");
                Serial.print(packet.localPort());
                Serial.print(", Length: ");
                Serial.print(packet.length());
                Serial.println();

                S_HOTSHOT_PACKET_RESPONSE response;
                memcpy(&response, packet.data(), sizeof(response));
                Serial.printf("checkcode: %u \n", response.checkcode);
            });
    }
    else {
        Serial.println("UDP Listen failed");
    }

    g_ts.startNow();

}

void loop()
{
    g_ts.execute();

}
