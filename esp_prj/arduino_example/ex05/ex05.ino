//
// A simple server implementation showing how to:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//

#include <Arduino.h>
// #ifdef ESP32
// #include <WiFi.h>
// #include <AsyncTCP.h>
// #elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
// #endif
#include <ESPAsyncWebServer.h>

#include <FS.h>

AsyncWebServer server(80); // create webserver object on port 80

const char *ssid = "redstar0427";
const char *password = "123456789a";

const char *PARAM_MESSAGE = "message";

void notFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Not found");
}

const int ledPin = 12;
void setup()
{
    pinMode(BUILTIN_LED, OUTPUT);
    pinMode(ledPin, OUTPUT);

    digitalWrite(BUILTIN_LED, LOW);
    digitalWrite(ledPin, LOW);

    Serial.begin(115200);

    while (!Serial) // wait for serial port to connect. Needed for native USB port only
        ;
    
    Serial.println("\r\n\r\nSerial connected");
    

    // connect wifi network
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.printf("WiFi Failed!\n");
        return;
    }

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // mount file system
    if (!SPIFFS.begin())
    {
        // Serious problem
        Serial.println("SPIFFS Mount failed");
        return;
    }
    else
    {
        Serial.println("SPIFFS Mount succesfull");
    }

    
    // Route for root / web page
    // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    //           { request->send(200, "text/plain", "Hello, world"); });

    // Send a GET request to <IP>/get?message=<message>
    server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        String message;
        if (request->hasParam(PARAM_MESSAGE)) {
            message = request->getParam(PARAM_MESSAGE)->value();

            if(message == "on")
            {
                digitalWrite(ledPin, HIGH);
            }
            else if(message == "off")
            {
                digitalWrite(ledPin, LOW);
            }

        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, GET: " + message); });

    // Send a POST request to <IP>/post with a form field message set to <message>
    server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request)
              {
        String message;
        if (request->hasParam(PARAM_MESSAGE, true)) {
            message = request->getParam(PARAM_MESSAGE, true)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, POST: " + message); });
    


    // static file
    server.serveStatic("/", SPIFFS, "/www/").setDefaultFile("index.html");

    server.onNotFound(notFound);

    server.begin();

    digitalWrite(BUILTIN_LED, HIGH);
}

void loop()
{

    // digitalWrite(BUILTIN_LED, LOW);
    // delay(5000);
    // digitalWrite(BUILTIN_LED, HIGH);
    // delay(5000);
}