#include <Arduino.h> // Arduino 헤더 파일 포함
#include <TaskScheduler.h>
// TTGO T1B V1.3 보드의 내장 LED는 GPIO 5번 핀에 연결되어 있습니다.
#define LED_BUILTIN 5 // GPIO 핀 번호에 따라 변경할 수 있습니다.

//0,2,4,12,15 는 사용금지
//21,22 i2c

int pins[] = {16,17,18,19,13};
int num_pins = sizeof(pins) / sizeof(pins[0]);
int delay_time = 500; // 1초

Scheduler g_ts_blinker;
Scheduler g_ts_ledtest;

Task task_Test(
    delay_time, TASK_FOREVER, []()
    {
      static int i = 0;
      digitalWrite(pins[i], !digitalRead(pins[i]));
      Serial.print("Pin ");
      Serial.print(pins[i]);
      if (digitalRead(pins[i]))
        Serial.println(" is On");
      else
        Serial.println(" is off");


      i = (i + 1) % num_pins;
    },
    &g_ts_ledtest, true);

Task task_Blink(1000, TASK_FOREVER, []() {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}, &g_ts_blinker, true);

void setup()
{
    Serial.begin(115200);         // 시리얼 통신 초기화
    pinMode(LED_BUILTIN, OUTPUT); // 내장 LED를 출력 모드로 설정

    for(int i=0; i<10; i++){
        pinMode(pins[i], OUTPUT);
        digitalWrite(pins[i], LOW);
    }
    

    while (!Serial)
        ; // wait for serial attach
    delay(500);

    Serial.println(":-]");
    Serial.println("Serial connected");

    g_ts_blinker.startNow();
    g_ts_ledtest.startNow();

}

void loop() {
    g_ts_blinker.execute();
    g_ts_ledtest.execute();
  
}