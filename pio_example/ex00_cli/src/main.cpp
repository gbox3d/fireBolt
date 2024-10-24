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
#include "etc.hpp"

Scheduler g_ts;
Config g_config;

extern String parseCmd(String _strLine);

// #ifdef ESP32
// #define LED_BUILTIN 4
// #endif

#if defined(LOLIN_D32) | defined(LOLIN_D32_PRO) | defined(WROVER_KIT)
// this device aready defined LED_BUILTIN 4 -> D5 

#elif defined(SEED_XIAO_ESP32C3)
#define LED_BUILTIN D10

#endif


Task task_LedBlink(500, TASK_FOREVER, []()
              {
                  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
              }, &g_ts, true);

Task task_Cmd(100, TASK_FOREVER, []()
              {

#if defined(__AVR_ATmega2560__)

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
        
        // Serial.println(_strLine);

        Serial.println(parseCmd(_strLine));
    } }, &g_ts, true);

// the setup function runs once when you press reset or power the board
void setup()
{
  // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); // turn the LED off by making the voltage LOW

    Serial.begin(115200);

    g_config.load();

    delay(250);

    Serial.println(":-]");
    Serial.println("Serial connected");
    
    Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
    Serial.printf("Chip ID: %s\n", getChipID().c_str());

    //보드에 대한 정보를 출력
    Serial.printf("Board: %s\n", ARDUINO_BOARD);
    Serial.println("LED_BUILTIN: " + String(LED_BUILTIN));
    
    
    Serial.printf("Config System Revision: %d\n", Config::SystemVersion);
    int _value = g_config.get<int>("value", 727);
    String _name = g_config.get<String>("name", "dallos");

    Serial.printf("value: %d\n", _value);
    Serial.printf("name: %s\n", _name.c_str());

    Serial.println("Config loaded and Test ok");

#if defined(__AVR_ATmega2560__)
    Serial1.begin(115200);
    Serial1.println(":-]");
    Serial1.println("Serial1 connected");
#endif
    //task manager start
    g_ts.startNow();
}

// the loop function runs over and over again forever
void loop()
{
  g_ts.execute();
}