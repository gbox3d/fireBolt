#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>

const char *ssid = "";
const char *password = "";

ESP8266WebServer server(80);

// Serving Hello world
void getHelloWord()
{
    server.send(200, "text/json", "{\"name\": \"Hello world\"}");
}

// Define routing
void restServerRouting()
{
    // Route for root / web page
    // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    //     request->send(SPIFFS, "/index.html", String(), false, processor);
    // });

    // server.on("/", HTTP_GET, []()
    //           { server.send(200, F("text/html"),
    //                         F("Welcome to the REST Web Server")); });

    server.serveStatic("/img", SPIFFS, "/img");
    server.serveStatic("/js", SPIFFS, "/js");
    server.serveStatic("/css", SPIFFS, "/css");
    server.serveStatic("/", SPIFFS, "/index.html");
    server.on(F("/helloWorld"), HTTP_GET, getHelloWord);
}

// Manage not found URL
void handleNotFound()
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}

void setup(void)
{
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    if (!SPIFFS.begin())
    {
        // Serious problem
        Serial.println("SPIFFS Mount failed");
    }
    else
    {
        Serial.println("SPIFFS Mount succesfull");
    }

    // Activate mDNS this is used to be able to connect to the server
    // with local DNS hostmane esp8266.local
    // if (MDNS.begin("esp8266"))
    // {
    //     Serial.println("MDNS responder started");
    // }

    // Set server routing
    restServerRouting();
    // Set not found response
    server.onNotFound(handleNotFound);
    // Start server
    server.begin();
    Serial.println("HTTP server started");
}

void loop(void)
{
    server.handleClient();
    delay(100);
}