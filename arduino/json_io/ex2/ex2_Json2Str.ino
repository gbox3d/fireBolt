#include <Arduino.h>
#include <ArduinoJson.h>

void setup()
{
    Serial.begin(115200);

    DynamicJsonDocument doc(1024);

    doc["sensor"] = "gps";
    doc["time"]   = 1351824120;
    doc["data"][0] = 48.756080;
    doc["data"][1] = 2.302038;

    serializeJson(doc, Serial);
    
}

void loop()
{
    
}