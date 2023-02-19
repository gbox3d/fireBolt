#include <Arduino.h>
#include <ESP8266WiFi.h>
// #include <ESPAsyncTCP.h>
// #include <ESPAsyncWebServer.h>

#include <FS.h>
#include <ArduinoJson.h>
#include <ESPAsyncUDP.h>

#include "config.hpp"


CespConfig g_config;

#define PIN_TRIGER 14
#define PIN_LED 12


// AsyncClient* gpClient = NULL;
AsyncUDP udp;
// IPAddress targetIP;
// AsyncUDPMessage udpMessage;

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(PIN_LED, OUTPUT);
    pinMode(PIN_TRIGER, INPUT);
    
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(PIN_LED, LOW);

    // put your setup code here, to run once:
    Serial.begin(115200);
    // while (!Serial);
    delay(1000); //cool time
    Serial.println(":-]");
    Serial.println("Serial connected");

    //mount file system
    if (!SPIFFS.begin())
    {
        Serial.println("SPIFFS Mount failed");
        return;
    }
    else
    {
        Serial.println("SPIFFS Mount succesfull");
    }

    //parse config.json
    if (SPIFFS.exists("/config.json"))
    {
        //file exists, reading and loading
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
    // targetIP.fromString(g_config.remoteIp);

    Serial.println("version: " + String(g_config.version));
    Serial.println("ssid: " + g_config.ssid);
    Serial.println("passwd: " + g_config.passwd);
    Serial.printf("remote ip : %s \n\n", g_config.remoteIp.toString().c_str());

    //connect to wifi
    WiFi.mode(WIFI_STA);
    WiFi.begin(g_config.ssid, g_config.passwd);
    Serial.println("Connecting to WiFi..");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected to the WiFi network");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    digitalWrite(LED_BUILTIN, LOW);

//setup udp
//   "{\"cmd\":\"triger\",\"index\":" + String(g_config.index) + "}\r\n").c_str())
// udpMessage.


    


    

}

int g_nFSM = 0;
void loop()
{

    switch(g_nFSM)
    {
        case 0:
        {
            if (digitalRead(PIN_TRIGER) == HIGH)
            {
                g_nFSM = 1;

                

                AsyncUDPMessage udpMessage;
                udpMessage.printf("{\"cmd\":\"triger\",\"index\":%d}\r\n",g_config.index);
                // udp.broadcastTo(udpMessage, g_config.port);
                // udp.sendTo(udpMessage, 
                //     IPAddress( (const uint8_t *) g_config.remoteIp.c_str()),
                //     g_config.remotePort);

                // udp.broadcastTo(udpMessage, g_config.remotePort);

                udp.sendTo(udpMessage, 
                    g_config.remoteIp,
                    g_config.remotePort);

                Serial.println(g_config.remoteIp);
                Serial.println(g_config.remotePort);

                digitalWrite(PIN_LED, LOW);
                delay(50);
                digitalWrite(PIN_LED, HIGH);

            }
            break;
        }
        case 1:
        {
            if (digitalRead(PIN_TRIGER) == LOW)
            {
                g_nFSM = 0;
                delay(50);
                // digitalWrite(PIN_LED, LOW);
            }
            break;
        }
    }
    
}
