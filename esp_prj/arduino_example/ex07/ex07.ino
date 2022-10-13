#include <Arduino.h>
#include <ESP8266WiFi.h>
// #include <ESPAsyncTCP.h>
// #include <ESPAsyncWebServer.h>

#include <ESPAsyncUDP.h>

#include <FS.h>
#include <ArduinoJson.h>

#include "config.hpp"

CespConfig g_config;

AsyncUDP udp;

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    // while (!Serial);
    delay(1000); // cool time
    Serial.println(":-]");
    Serial.println("Serial connected");

    // mount file system
    if (!SPIFFS.begin())
    {
        Serial.println("SPIFFS Mount failed");
        return;
    }
    else
    {
        Serial.println("SPIFFS Mount succesfull");
    }

    // parse config.json
    if (SPIFFS.exists("/config.json"))
    {
        // file exists, reading and loading
        Serial.println("reading config file");
        File configFile = SPIFFS.open("/config.json", "r");
        if (configFile)
        {
            Serial.println("opened config file");
            g_config.load(configFile);

            // deserializeJson(jsonBuffer, configFile);
        }

        configFile.close();
    }
    else
    {
        Serial.println("config file not found");
        return;
    }

    Serial.println("config file loaded");
    Serial.println("version: " + String(g_config.version));
    Serial.println("ssid: " + g_config.ssid);
    Serial.println("passwd: " + g_config.passwd);

    WiFi.mode(WIFI_STA);
    WiFi.begin(g_config.ssid, g_config.passwd);

    if (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.printf("WiFi Failed!\n");
        return;
    }

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    //udp listening
    if(udp.listen(8060)) {
        Serial.print("UDP Listening on IP: ");
        Serial.println(WiFi.localIP());
        udp.onPacket([](AsyncUDPPacket packet) {
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
            Serial.print(", Data: ");
            Serial.write(packet.data(), packet.length());
            Serial.println();
            //reply to the client
            // packet.printf("Got %u bytes of data", packet.length());
        });
    }
    
    // udp.onPacket([](AsyncUDPPacket packet) {
    //     Serial.printf("UDP Packet Type: %d, From: %s, To: %s, Len: %d, Data: %s",
    //      packet.isBroadcast() ? 1 : 0, packet.remoteIP().toString().c_str(), packet.localIP().toString().c_str(), packet.length(), packet.data());
    // });

}

void loop()
{
    delay(3000);
    //Send broadcast on port ...
    udp.broadcastTo("{\"pkt\":\"brdcast\",\"msg\":\"a u there?\" }", 50020);
}
