#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "EXTERNAL_CLOCK";

void app_main(void)
{
    esp_err_t err;

    // ------------------------------
    // 1. BCK (Bit Clock) 생성: 1.6 MHz, 출력 GPIO18
    // ------------------------------
    ledc_timer_config_t ledc_timer_bck = {
        .speed_mode       = LEDC_HIGH_SPEED_MODE,
        .timer_num        = LEDC_TIMER_0,
        .duty_resolution  = LEDC_TIMER_2_BIT,   // 2비트: 0 ~ 3
        .freq_hz          = 1600000,            // 1.6 MHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    err = ledc_timer_config(&ledc_timer_bck);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "BCK 타이머 설정 실패");
    }

    ledc_channel_config_t ledc_channel_bck = {
        .speed_mode     = LEDC_HIGH_SPEED_MODE,
        .channel        = LEDC_CHANNEL_0,
        .timer_sel      = LEDC_TIMER_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = 18,       // BCK 출력 핀
        .duty           = 2,        // 2비트 기준 50% 듀티 (0~3 중 약 50%)
        .hpoint         = 0
    };
    err = ledc_channel_config(&ledc_channel_bck);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "BCK 채널 설정 실패");
    }

    // ------------------------------
    // 2. WS (Word Select) 생성: 100 kHz, 출력 GPIO19
    //    기존 10비트 해상도 대신 8비트 해상도로 변경하여 분주기 계산 문제 해결
    // ------------------------------
    ledc_timer_config_t ledc_timer_ws = {
        .speed_mode       = LEDC_HIGH_SPEED_MODE,
        .timer_num        = LEDC_TIMER_1,
        .duty_resolution  = LEDC_TIMER_8_BIT,   // 8비트 해상도 사용 (0~255)
        .freq_hz          = 100000,             // 100 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    err = ledc_timer_config(&ledc_timer_ws);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "WS 타이머 설정 실패");
    }

    ledc_channel_config_t ledc_channel_ws = {
        .speed_mode     = LEDC_HIGH_SPEED_MODE,
        .channel        = LEDC_CHANNEL_1,
        .timer_sel      = LEDC_TIMER_1,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = 19,       // WS 출력 핀
        .duty           = 128,      // 8비트 기준 50% 듀티 (128/255 ≒ 50%)
        .hpoint         = 0
    };
    err = ledc_channel_config(&ledc_channel_ws);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "WS 채널 설정 실패");
    }

    ESP_LOGI(TAG, "GPIO18(BCK, 약 1.6MHz)와 GPIO19(WS, 약 100kHz)에서 외부 클럭 신호 생성 중");

    // 무한 루프: 태스크가 종료되지 않도록 함.
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
