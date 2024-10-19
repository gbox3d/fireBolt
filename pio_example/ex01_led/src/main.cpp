#include <Arduino.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif ESP32
#include <WiFi.h>
#include <vector>
#endif

#include <TaskScheduler.h>
#include <ArduinoJson.h>

// #include "tonkey.hpp"

#include "config.hpp"

Scheduler g_ts;
// tonkey g_MainParser;

Config g_config;

std::vector<int> ledPins;

void ledOffCallback();
void ledOnCallback();
Task tLedBlinker(500, TASK_FOREVER, &ledOnCallback, &g_ts, false);

#ifdef ESP32

#if not defined(LED_BUILTIN) // check if the default LED pin is defined
#define LED_BUILTIN 5 // define the default LED pin if it isn't defined
#endif

#endif

void ledOnCallback()
{
    digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
    tLedBlinker.setCallback(&ledOffCallback);
}

void ledOffCallback()
{
    digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
    tLedBlinker.setCallback(&ledOnCallback);
    // Serial.println("led blink");
    // Serial.println(LED_BUILTIN);
}

extern String parseCmd(String _strLine);

Task task_Cmd(100, TASK_FOREVER, []()
              {

#ifdef MEGA2560

    if(Serial1.available() > 0) {
        String _strLine = Serial1.readStringUntil('\n');
        _strLine.trim();
        Serial1.println(_strLine);
    }
#endif

    if (Serial.available() > 0)
    {
        String _strLine = Serial.readStringUntil('\n');
        _strLine.trim();
        Serial.println(parseCmd(_strLine));

    } }, &g_ts, true);

void startBlink()
{
    tLedBlinker.enable();
}

void stopBlink()
{
    tLedBlinker.disable();
}

void ledOn(int pinIndex)
{
    if(pinIndex == -1) {
        for(int i = 0; i < ledPins.size(); i++) {
            int pin = ledPins[i];
            digitalWrite(pin, HIGH); // turn the LED on (HIGH is the voltage level)
        }
    }
    else {
        int pin = ledPins[pinIndex];
        digitalWrite(pin, HIGH); // turn the LED on (HIGH is the voltage level)
    }
}

void ledOff(int pinIndex)
{
    if(pinIndex == -1) {
        for(int i = 0; i < ledPins.size(); i++) {
            int pin = ledPins[i];
            digitalWrite(pin, LOW); // turn the LED on (HIGH is the voltage level)
        }
    }
    else {
        int pin = ledPins[pinIndex];
        digitalWrite(pin, LOW); // turn the LED on (HIGH is the voltage level)
    }
}



// the setup function runs once when you press reset or power the board
void setup()
{
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); // turn the LED off by making the voltage LOW

    Serial.begin(115200);

    g_config.load();

    delay(500);

    JsonDocument _doc_ledpins;
    g_config.getArray("ledpin", _doc_ledpins);

    JsonArray ledpin = _doc_ledpins.as<JsonArray>();

    for (JsonVariant v : ledpin)
    {

        int pin = v.as<int>();

        pinMode(pin, OUTPUT);
        digitalWrite(pin, HIGH); // turn the LED off by making the voltage LOW
        ledPins.push_back(pin);

        Serial.print("led pin : " + String(pin) + "\n");
    }
    Serial.println(":-]");
    Serial.println("Serial connected");
    Serial.println("LED_BUILTIN: " + String(LED_BUILTIN));
    Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
    

    startBlink();


#ifdef ESP8266
    Serial.println("ESP8266");
#endif

    g_ts.startNow();
}

// the loop function runs over and over again forever
void loop()
{
    g_ts.execute();
}