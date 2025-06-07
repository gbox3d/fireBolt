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

#include "A3144.hpp"

extern String parseCmd(String _strLine);

Scheduler g_ts;
Config g_config;


#ifdef SEED_XIAO_ESP32C3
#define LED_BUILTIN D10
#define SENSOR_PIN D8


#elif defined(TENSTAR_ESP32C3)

#define LED_BUILTIN 8
#define SENSOR_PIN 0

#elif LOLIN_D32
#define SENSOR_PIN 4
#endif

Task task_Cmd(100, TASK_FOREVER, []()
              {

    if (Serial.available() > 0)
    {
        String _strLine = Serial.readStringUntil('\n');
        _strLine.trim();
        
        // Serial.println(_strLine);

        Serial.println(parseCmd(_strLine));
    } }, &g_ts, true);



Task Task_Sensor(100, TASK_FOREVER, []()
{
    static int oldValue = 0;
    int _value = A3144::getTriggerCount();
    if (_value != oldValue)
    {
        Serial.println("Magnetic Sensor: " + String(_value));
        oldValue = _value;
    }

    // Serial.println("Sensor Value: " + String(A3144::getSensorValue()));

    if(A3144::getSensorValue() == 0) {
        digitalWrite(LED_BUILTIN, LOW);
    }
    else {
        digitalWrite(LED_BUILTIN, HIGH);
    }
    
}, &g_ts, false);


void startSensorTask()
{
    Task_Sensor.enable();
}

void stopSensorTask()
{
    Task_Sensor.disable();
}

// the setup function runs once when you press reset or power the board
void setup()
{
  // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); // turn the LED off by making the voltage LOW

    A3144::setup(SENSOR_PIN);

    Serial.begin(115200);

    g_config.load();

    Serial.println(":-]");
    Serial.println("Serial connected");

    startSensorTask();
    
    g_ts.startNow();
}

// the loop function runs over and over again forever
void loop()
{
  g_ts.execute();
}