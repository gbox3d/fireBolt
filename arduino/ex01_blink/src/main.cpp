#include <Arduino.h>
#include <TaskScheduler.h>
#include <tonkey.hpp>


Scheduler g_ts;
tonkey g_MainParser;
// CCongifData configData;

Task task_Cmd(
    100, TASK_FOREVER, []()
    {
    if (Serial.available() > 0)
    {
        String _strResult ="";
        String _strLine = Serial.readStringUntil('\n');
        _strLine.trim();
        Serial.println(_strLine);
        g_MainParser.parse(_strLine);

        if(g_MainParser.getTokenCount() > 0) {
          String cmd = g_MainParser.getToken(0);
            
            if (cmd == "about")
            {
              _strResult = "it is blank sample \nok\n";
            }
        }
        Serial.println(_strResult);        
    }
    }
);

Task task_blink(
    200, TASK_FOREVER, []()
    {
        // digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        Serial.println("blink");
    }
);

void setup() {
  // put your setup code here, to run once:
  // pinMode(LED_BUILTIN, OUTPUT);
  // digitalWrite(LED_BUILTIN, HIGH);

  Serial.begin(115200);

  Serial.println("fireBolt ex01_blink");
  g_ts.init();
  g_ts.addTask(task_Cmd);
  g_ts.addTask(task_blink);

  task_Cmd.enable();
  task_blink.enable();
  
}

void loop() {
  // put your main code here, to run repeatedly:

  g_ts.execute();
}

