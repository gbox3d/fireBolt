#include <Arduino.h>

#include <TaskScheduler.h>

#if defined(LOLIN_D32)
// TTGO T1B V1.3 보드의 내장 LED는 GPIO 5번 핀에 연결되어 있습니다.
#define LED_BUILTIN 5 // GPIO 핀 번호에 따라 변경할 수 있습니다.
int pins[] = {13, 14, 18, 19, 23, 25, 26, 27, 32, 33};
// 0,2,4,12,15 는 사용금지
// 21,22 i2c
const int ledPins[] = {4, 5};

#elif defined(SEED_XIAO_ESP32C3)
#define LED_BUILTIN D10 // GPIO 핀 번호에 따라 변경할 수 있습니다.
int pins[] = {D10, D9};

#elif defined(LOLIN_D32_LITE)
const int ledPins[] = {22};
#endif

int num_pins = sizeof(pins) / sizeof(pins[0]);
int delay_time = 500; // 1초

// const int interruptPin = 14;

// Scheduler g_ts_blinker;
Scheduler runner;


Task task_Blink(
    1000, TASK_FOREVER, []()
    { 
      for(int i = 0; i < num_pins; i++){
        digitalWrite(pins[i], !digitalRead(pins[i]));
      }
    } 
    );

// volatile bool interruptFlag = false;

// void IRAM_ATTR isr()
// {
//   interruptFlag = true;
// }

void setup()
{
  Serial.begin(115200); // 시리얼 통신 초기화

  for (int i = 0; i < num_pins; i++)
  {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW);
  }

  //주의 : 아래 코드는 씨리얼 연결될때까지 대기 그래서 연ㅕㄹ되지않으면 다음 단계로 넘어가지 않음, 그래서 외부전원만 연결되어있으면 다음단계로 넘어가지 않음
  // while (!Serial)
  //   ; // wait for serial attach (for Leonardo/Micro/Zero)

  Serial.println(":-]");
  Serial.println("Serial connected");

  runner.init();
  runner.addTask(task_Blink);
  task_Blink.enable();
  
}

void loop()
{
  runner.execute();
}