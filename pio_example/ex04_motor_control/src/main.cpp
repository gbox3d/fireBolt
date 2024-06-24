#include <Arduino.h> // Arduino 헤더 파일 포함
#include <TaskScheduler.h>
#include <ArduinoJson.h>
#include <tonkey.hpp>

// for motor controler tb6612fng
const int AIN_1 = D0;
const int AIN_2 = D1;
const int A_PWM_PIN = D2; // 0~255
const int STAND_BY = D3;  // vcc 에 연결하면 standby 모드가 된다. 신호선에 연결하면 동작하지않는다.

Scheduler g_ts;
tonkey g_MainParser;

Task task_Cmd(
    100, TASK_FOREVER, []()
    {
    if (Serial.available() > 0)
    {
        String _strLine = Serial.readStringUntil('\n');
        _strLine.trim();
        Serial.println(_strLine);
        g_MainParser.parse(_strLine);

        if(g_MainParser.getTokenCount() > 0) {
          String cmd = g_MainParser.getToken(0);
            DynamicJsonDocument _res_doc(1024);

            if (cmd == "forward")
            {
              Serial.println("forward");
              digitalWrite(AIN_1, HIGH);
              digitalWrite(AIN_2, LOW);
              analogWrite(A_PWM_PIN, 128);
            }
            else if (cmd == "backward")
            {
              Serial.println("backward");
              digitalWrite(AIN_1, LOW);
              digitalWrite(AIN_2, HIGH);
              analogWrite(A_PWM_PIN, 128);
            }
            else if (cmd == "stop")
            {
              Serial.println("stop");
              digitalWrite(AIN_1, LOW);
              digitalWrite(AIN_2, LOW);
              analogWrite(A_PWM_PIN, 0);
            }
            else if (cmd == "wakeup")
            {
              Serial.println("standby");
              digitalWrite(STAND_BY, HIGH);
            }
            else if (cmd == "sleep")
            {
              Serial.println("sleep");
              digitalWrite(STAND_BY, LOW);
            }
            else if (cmd == "slow")
            {
              Serial.println("slow");
              analogWrite(A_PWM_PIN, 128);
            }
            else if (cmd == "fast")
            {
              Serial.println("fast");
              analogWrite(A_PWM_PIN, 255);
            }
            
            else if (cmd == "help")
            {
              Serial.println("command list");
              Serial.println("forward : forward");
              Serial.println("backward : backward");
              Serial.println("stop : stop");
            }
            else
            {
              Serial.println("unknown command");
            }
        }
    } }
    ,&g_ts, true);

void setup()
{
  pinMode(AIN_1, OUTPUT);
  pinMode(AIN_2, OUTPUT);
  pinMode(A_PWM_PIN, OUTPUT);
  pinMode(STAND_BY, OUTPUT);

  Serial.begin(115200);
  while (!Serial)
    ; // wait for Leonardo enumeration, others continue immediately

  Serial.println("ready to go!");
  g_ts.startNow();
}

void loop()
{
  g_ts.execute();
  // processSerialCmder(doCommand);
}