#include <Arduino.h>
#include <TaskScheduler.h>

#include <tonkey.hpp>

#include "config.hpp"

#if defined(LOLIN_D32)
  #define LED_BUILTIN 5
  const int ledPins[] = {4,5};
#elif defined(SEED_XIAO_ESP32C3)
  const int ledPins[] = {D10,D9};
  const int analogPins[] = {D0,D1};
  const int buttonPins[] = {D8,D2,D3};
#elif defined(LOLIN_D32_LITE)
  const int ledPins[] = {22};
  
#endif

String strTitleMsg = "it is MOAI(motion + ai) framework sample revision 1";

String strHelpMsg = "command list\n\
help : show this message\n\
led on|off|toggle|pwm <index> <value> : control led\n\
button <index> : read button\n\
analog <index> : read analog\n\
config mNumber <value> : set mNumber\n\
config mMsg <value> : set mMsg\n\
save : save config\n\
load : load config\n\
clear : clear config\n\
reboot : reboot esp32\n\
print : print config\n\
";

CCongifData g_Config;
tonkey g_MainParser;
Scheduler runner;

Task task_Cmd(
    100, TASK_FOREVER, []()
    {
    if (Serial.available() > 0)
    {
        String _strLine = Serial.readStringUntil('\n');
        _strLine.trim();
        // Serial.println(_strLine);
        g_MainParser.parse(_strLine);

        if(g_MainParser.getTokenCount() > 0) {
          String cmd = g_MainParser.getToken(0);
          String _result = "OK";

          if(cmd=="help") {
            Serial.println(strTitleMsg);
            Serial.println(strHelpMsg);
          }
          else if(cmd == "led") {
            if(g_MainParser.getTokenCount() > 2) {
              String _strLed = g_MainParser.getToken(1);
              int _index = g_MainParser.getToken(2).toInt();
              if(_strLed == "on") {
                digitalWrite(ledPins[_index], HIGH);
              }
              else if(_strLed == "off") {
                digitalWrite(ledPins[_index], LOW);
              }
              else if(_strLed == "toggle") {
                digitalWrite(ledPins[_index], !digitalRead(ledPins[_index]));
              }
              else if(_strLed == "pwm") {
                if(g_MainParser.getTokenCount() > 3) {
                  int _value = g_MainParser.getToken(3).toInt();
                  analogWrite(ledPins[_index], _value);
                }
                else {
                  _result = "FAIL";
                }
              }
              else {
                _result = "FAIL";
              }
            }
            else {
              _result = "FAIL";
            }
          }
          else if(cmd=="button") {
            if(g_MainParser.getTokenCount() > 1) {
              
              int _index = g_MainParser.getToken(1).toInt();
              int _value = digitalRead(buttonPins[_index]);
              _result = String(_value) + "\nOK";
            }
            else {
              _result = "FAIL";
            }
          }
          else if(cmd=="analog") {
            if(g_MainParser.getTokenCount() > 1) {
              
              int _index = g_MainParser.getToken(1).toInt();
              int _value = analogRead(analogPins[_index]);
              _result = String(_value) + "\nOK";
            }
            else {
              _result = "FAIL";
            }
          }
          else if(cmd=="config") {
            if(g_MainParser.getTokenCount() > 2) {
              String _strKey = g_MainParser.getToken(1);
              String _strValue = g_MainParser.getToken(2);
              if(_strKey == "mNumber") {
                g_Config.mNumber = _strValue.toInt();
              }
              else if(_strKey == "mMsg") {
                g_Config.mMsg = _strValue;
              }
              else {
                _result = "FAIL";
              }
            }
          }
          else if(cmd=="save") {
            g_Config.save();
          }
          else if(cmd=="load") {
            g_Config.load();
          }
          else if(cmd=="clear") {
            g_Config.clear();
          }
          else if(cmd=="reboot") {
            ESP.restart();
          }
          else if(cmd=="print") {
            g_Config.print();
          }

          Serial.println(_result);
        }
    }
    });

void setup() {

  //io setup
  for(int i=0; i<sizeof(ledPins)/sizeof(ledPins[0]); i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
  
  for(int i=0; i<sizeof(buttonPins)/sizeof(buttonPins[0]); i++) {
    pinMode(buttonPins[i], INPUT_PULLUP); //LOW: button pressed
  }

  for(int i=0; i<sizeof(analogPins)/sizeof(analogPins[0]); i++) {
    pinMode(analogPins[i], INPUT);
  }

  Serial.begin(115200);

  while(!Serial) 
    ;

  runner.init();
  runner.addTask(task_Cmd);
  task_Cmd.enable();

  delay(1000);
  Serial.println(strTitleMsg);
}

void loop() {
  runner.execute();
}
