#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <FS.h>
#include <ArduinoJson.h>

#include "config.hpp"

CespConfig g_config;

#define PIN_TRIGER 14
#define PIN_LED 12


AsyncClient* gpClient = NULL;

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
    // Serial.println("version: " + String(g_config.version));
    // Serial.println("ssid: " + g_config.ssid);
    // Serial.println("passwd: " + g_config.passwd);

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

//setup tcp
    AsyncClient* client = new AsyncClient;

	client->onData([](void* arg, AsyncClient* client, void *data, size_t len) {


    }, client);

	client->onConnect([](void* arg, AsyncClient* client) {
        Serial.println("Connected " + client->remoteIP().toString() + ":" + client->remotePort());
        // Serial.printf("\n client has been connected to %s on port %d \n", SERVER_HOST_NAME, TCP_PORT);
        // attachInterrupt(digitalPinToInterrupt(PIN_TRIGER), detectsTriger, FALLING);
        gpClient = client;
        digitalWrite(PIN_LED, HIGH);


    }, client);

    client->onDisconnect([](void* arg, AsyncClient* client) {
        Serial.println("Disconnected " + client->remoteIP().toString() + ":" + client->remotePort());
        digitalWrite(PIN_LED, LOW);
        // Serial.printf("\n client has been disconnected from %s on port %d \n", SERVER_HOST_NAME, TCP_PORT);
    }, client);

    Serial.println("Connecting to " + g_config.remoteIp + ":" + String(g_config.remotePort));

	client->connect(g_config.remoteIp.c_str(), g_config.remotePort);

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

                if(gpClient != NULL)
                {
                    gpClient->write(
                        ("{\"cmd\":\"triger\",\"index\":" + String(g_config.index) + "}\r\n").c_str()
                        );
                }

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
