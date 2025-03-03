#include <Arduino.h>
#include "driver/adc.h"
#include "esp_adc_cal.h"

// 아날로그 핀 설정 (ADC1 채널)
#define CHANNEL_COUNT 4
const adc1_channel_t ADC_CHANNELS[CHANNEL_COUNT] = {
    ADC1_CHANNEL_0,  // GPIO 36 (VP)
    ADC1_CHANNEL_3,  // GPIO 39 (VN)
    ADC1_CHANNEL_4,  // GPIO 32
    ADC1_CHANNEL_5   // GPIO 33
};

// 실제 GPIO 핀 번호 (참조용)
const int ADC_GPIO[CHANNEL_COUNT] = {36, 39, 32, 33};

// ADC 값 저장 변수
uint16_t adc_values[CHANNEL_COUNT];

// 마지막 읽기 시간
unsigned long last_read_time = 0;
const int READ_INTERVAL = 500; // 0.5초(500ms) 간격

void setup() {
    // 시리얼 통신 초기화
    Serial.begin(115200);
    delay(1000);  // 시리얼 연결 안정화 대기
    
    Serial.println("ESP32 0.5초 간격 ADC 읽기 예제");
    Serial.println("읽을 핀:");
    
    // ADC 초기화
    for (int i = 0; i < CHANNEL_COUNT; i++) {
        // ADC 설정 (12비트 해상도, 감쇠 없음)
        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(ADC_CHANNELS[i], ADC_ATTEN_DB_0);
        
        Serial.printf("채널 %d: GPIO %d (ADC1_CH%d)\n", 
                     i, ADC_GPIO[i], ADC_CHANNELS[i]);
    }
    
    Serial.println("시작...");
}

void loop() {
    // 현재 시간 확인
    unsigned long current_time = millis();
    
    // 0.5초(500ms)마다 ADC 값 읽기
    if (current_time - last_read_time >= READ_INTERVAL) {
        // 현재 시간 갱신
        last_read_time = current_time;
        
        // 모든 채널 읽기
        for (int i = 0; i < CHANNEL_COUNT; i++) {
            adc_values[i] = adc1_get_raw(ADC_CHANNELS[i]);
        }
        
        // 읽은 값 출력
        Serial.printf("시간: %lu ms, 값: ", current_time);
        for (int i = 0; i < CHANNEL_COUNT; i++) {
            Serial.printf("GPIO%d: %d, ", ADC_GPIO[i], adc_values[i]);
        }
        Serial.println();
    }
    
    // 짧은 지연 (CPU 부하 감소)
    delay(10);
}