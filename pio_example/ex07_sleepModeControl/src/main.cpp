#include <Arduino.h>
#include "esp_sleep.h"

// 마지막 입력 시간 추적
unsigned long lastInputTime = millis();
bool bSleepMode = false;

void setup()
{
  Serial.begin(115200); // 시리얼 통신 시작
  delay(1000);          // 시리얼 모니터 초기화 대기 (개발 편의성을 위함)

  // GPIO0을 입력으로 설정
  pinMode(GPIO_NUM_4, INPUT_PULLUP);
  pinMode(D10, OUTPUT);
  pinMode(D9, OUTPUT);

  digitalWrite(D10, LOW);
  digitalWrite(D9, LOW);

  // // GPIO0을 LOW 신호로 깨우기 위해 구성
  uint64_t bitmask = 1ULL << GPIO_NUM_4;
  esp_deep_sleep_enable_gpio_wakeup(bitmask, ESP_GPIO_WAKEUP_GPIO_LOW);
 
}

void loop()
{
  
  if (!bSleepMode)
  {
    if (lastInputTime + 10000 < millis())
    {
      Serial.println("ready to wait sleep mode...");
      delay(1000);
      // Serial.println("deep sleep mode start");
      bSleepMode = true;

      esp_deep_sleep_start(); // 깊은 수면 모드에서 깨어난 후 이 코드는 실행되지 않습니다. ESP32는 재시작됩니다.
      return;
    }

    digitalWrite(D10, HIGH);
    delay(1000);
    digitalWrite(D10, LOW);
    delay(1000);
    Serial.println("loop wait..." + String( millis() - lastInputTime));
  }
  else
  {
    
  }
}
