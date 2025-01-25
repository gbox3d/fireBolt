#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h" // ESP-IDF의 GPIO API 사용

#define LED_GPIO GPIO_NUM_5 // 포트 5번 정의

// 블링크 태스크 함수
void blinkTask(void *pvParameters) {
  // GPIO 핀을 출력 모드로 설정
  gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

  while (1) {
    // LED ON
    gpio_set_level(LED_GPIO, 1);
    vTaskDelay(pdMS_TO_TICKS(500)); // 500ms 대기 (FreeRTOS 사용)

    // LED OFF
    gpio_set_level(LED_GPIO, 0);
    vTaskDelay(pdMS_TO_TICKS(500)); // 500ms 대기
  }
}

void setup()
{
   // 블링크 태스크 생성
  xTaskCreate(
    blinkTask,        // 태스크 함수
    "Blink Task",     // 태스크 이름
    1024,             // 태스크 스택 크기
    NULL,             // 태스크 매개변수
    1,                // 태스크 우선순위
    NULL              // 태스크 핸들러 (필요하지 않으므로 NULL)
  );
}

void loop()
{
  
}
