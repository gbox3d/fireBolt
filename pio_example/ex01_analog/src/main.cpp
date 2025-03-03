#include <Arduino.h>
#include <driver/adc.h>
#include "esp_adc_cal.h"

// ADC 채널 설정 (ADC1의 채널 4 = GPIO32)
#define ADC_CHANNEL ADC1_CHANNEL_4
#define ADC_PIN 32

// 임계값 설정
#define THRESHOLD_BASE 4090
#define THRESHOLD_DELTA 500

// 프로그램 상태 변수
volatile uint32_t totalSamples = 0;
volatile uint32_t thresholdCount = 0;
volatile uint32_t lastThresholdTime = 0;

// 태스크 핸들
TaskHandle_t samplingTask;

// ADC 설정을 위한 함수
void setupFastADC() {
  // ADC 해상도를 12비트로 설정 (0-4095)
  adc1_config_width(ADC_WIDTH_BIT_12);
  
  // ADC 채널 감쇠 설정 (0-3.3V 범위)
  adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_11);
  
  // ADC 성능 최적화를 위한 설정
  analogSetClockDiv(1); // 최소 클럭 분할 설정
}

// 코어 0에서 실행될 ADC 샘플링 및 임계값 감시 태스크
void adcSamplingTask(void *parameter) {
  uint16_t currentValue;
  uint16_t prevValue = adc1_get_raw(ADC_CHANNEL); // 초기값
  
  while(true) {
    // 최소 오버헤드로 ADC 샘플링
    currentValue = adc1_get_raw(ADC_CHANNEL);
    totalSamples++;
    
    // 임계값 감시 - 4090 기준으로 ±100 초과 감지
    int diff = abs((int)currentValue - THRESHOLD_BASE);
    if (diff > THRESHOLD_DELTA) {
      // 임계값 초과 감지
      thresholdCount++;
      lastThresholdTime = millis();
      
      // 임계값 초과 출력
      Serial.println("=============== 임계값 초과 감지! ===============");
      Serial.print("기준값: ");
      Serial.print(THRESHOLD_BASE);
      Serial.print(", 현재값: ");
      Serial.print(currentValue);
      Serial.print(", 편차: ");
      Serial.println(diff);
      Serial.print("총 발생 횟수: ");
      Serial.println(thresholdCount);
      Serial.println("==================================================");
      
      // 임계값을 벗어난 상태에서 반복 알림을 방지하기 위해
      // 기준값 범위 내로 돌아올 때까지 대기
      while (abs((int)adc1_get_raw(ADC_CHANNEL) - THRESHOLD_BASE) > THRESHOLD_DELTA) {
        delay(1); // 값이 정상 범위로 돌아올 때까지 짧게 대기
      }
    }
    
    // 최대 속도로 샘플링하기 위해 지연 없음
    taskYIELD(); // 다른 태스크에게 CPU 양보
  }
}

void setup() {
  // 시리얼 통신 속도 설정
  Serial.begin(115200);
  
  Serial.println("ESP32 고속 ADC 임계값 감지기 시작");
  Serial.print("기준값: ");
  Serial.print(THRESHOLD_BASE);
  Serial.print(", 허용 편차: ±");
  Serial.println(THRESHOLD_DELTA);
  
  // 빠른 ADC 설정 적용
  setupFastADC();
  
  // 핀 모드 설정
  pinMode(ADC_PIN, INPUT);
  
  // ADC 샘플링 및 임계값 감시 태스크 생성 (코어 0에서 실행)
  xTaskCreatePinnedToCore(
    adcSamplingTask,     // 태스크 함수
    "ADC Sampling",      // 태스크 이름
    10000,               // 스택 크기
    NULL,                // 파라미터
    1,                   // 우선순위
    &samplingTask,       // 태스크 핸들
    0                    // 코어 0에서 실행
  );
  
  Serial.println("임계값 감시 태스크가 시작되었습니다.");
}

void loop() {
  // 메인 루프는 사용하지 않음 - 모든 작업은 태스크에서 처리
  delay(1000);
}