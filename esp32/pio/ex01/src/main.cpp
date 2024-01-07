#include <Arduino.h>  // Arduino 헤더 파일 포함
#include <TaskScheduler.h>
#include <ArduinoJson.h>
#include <tonkey.hpp>


Scheduler g_ts;
tonkey g_MainParser;

Task task_Cmd(100, TASK_FOREVER, []() {
    if (Serial.available() > 0)
    {
        String _strLine = Serial.readStringUntil('\n');
        _strLine.trim();
        Serial.println(_strLine);
        g_MainParser.parse(_strLine);

        if(g_MainParser.getTokenCount() > 0) {
          String cmd = g_MainParser.getToken(0);
            DynamicJsonDocument _res_doc(1024);

            if (cmd == "about")
            {
                /* code */
                _res_doc["result"] = "ok";
                _res_doc["title"] = "example 01 - hello esp32";
                _res_doc["version"] = "1.0.0";
                _res_doc["author"] = "gbox3d";
            }
            
            else {
              _res_doc["result"] = "fail";
              _res_doc["ms"] = "unknown command";
            }

            serializeJson(_res_doc, Serial);
        }
    }
}, &g_ts, true);


void setup() {  
  Serial.begin(115200);  // 시리얼 통신 초기화

  while (!Serial); // wait for serial attach
    delay(500);

  Serial.println(":-]");
  Serial.println("Serial connected");
  
  g_ts.startNow();
}

void loop() {
    g_ts.execute();
}
