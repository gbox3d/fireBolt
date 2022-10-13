#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <FS.h>
#include <ArduinoJson.h>

#include "config.hpp"

CespConfig g_config;

void setup()
{
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
    Serial.println("version: " + String(g_config.version));
    Serial.println("ssid: " + g_config.ssid);
    Serial.println("passwd: " + g_config.passwd);
    
}

void loop()
{
    // put your main code here, to run repeatedly:
}
