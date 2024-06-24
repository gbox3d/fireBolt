#include <Arduino.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif ESP32
#include <WiFi.h>
#else

#endif

#include <Wire.h>

#include <TaskScheduler.h>
#include <ArduinoJson.h>

#include "tonkey.hpp"

#include "config.hpp"

Scheduler g_ts;
tonkey g_MainParser;
Config g_config;

#ifdef ESP32
#define LED_BUILTIN 4
#endif

//
// char output[64];

String parseCmd(String _strLine)
{
  String _strResult = "";

  _strLine.trim();
  Serial.println(_strLine);

  g_MainParser.parse(_strLine);

  if (g_MainParser.getTokenCount() > 0)
  {
    String cmd = g_MainParser.getToken(0);
    
    // JsonDocument _res_doc;
    if (cmd == "about")
    {
      _strResult = "it is i2c slave";
    }
    else
    {
      _strResult = "unknown command";
      // _res_doc["result"] = "fail";
      // _res_doc["ms"] = "unknown command";
    }

    // Serial.println(_strResult);
    // serializeJson(_res_doc, output);
  }
  else
  {
    _strResult = "nop";
  }

  return _strResult;
}

Task task_Cmd(100, TASK_FOREVER, []()
              {
    if (Serial.available() > 0)
    {
        String _strLine = Serial.readStringUntil('\n');

        Serial.println( parseCmd(_strLine) );
        
    } }, &g_ts, true);

String resData = "";

// the setup function runs once when you press reset or power the board
void setup()
{
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW

  Serial.begin(115200);

  Wire.begin(0x42); // join i2c bus with address 0x42

  
  Wire.onReceive([](int _byteCount) {

    String _strLine = "";
    while (Wire.available())
    {
      //개행문자가 들어오면 명령어를 실행한다.
      char c = Wire.read();
      if (c == '\n')
      {
        Serial.println( "proc cmd : " + _strLine);
        resData = parseCmd(_strLine);
        _strLine = "";
      }
      else
      {
        _strLine += c;
      }
    } 
  });

  Wire.onRequest([]() {

    // String _strLine = "hello";
    
// #ifdef DEBUG
//     Serial.print("transmit ");
//     Serial.print(_strLine.length());
//     Serial.println(" bytes");
// #endif

    Wire.write( resData.length());
    Wire.write(resData.c_str(), resData.length());
    
    
  });

  g_config.load();

  Serial.println(":-]");
  Serial.println("Serial connected");

  g_ts.startNow();
}

// the loop function runs over and over again forever
void loop()
{
  g_ts.execute();
}