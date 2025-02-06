#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include "string.h"

// I2S 설정
#define I2S_NUM         I2S_NUM_0
// 31,250 Hz 샘플레이트: 스테레오 16비트일 경우 BCK = 31,250 * 16 * 2 = 1MHz
#define SAMPLE_RATE     31250
#define I2S_BITS        I2S_BITS_PER_SAMPLE_16BIT
#define I2S_CHANNEL_FMT I2S_CHANNEL_FMT_RIGHT_LEFT
#define I2S_COMM_FMT    I2S_COMM_FORMAT_I2S

// 클럭 핀 (BCK: 18, WS: 19)
// 데이터 출력은 사용하지 않으므로 I2S_PIN_NO_CHANGE로 설정합니다.
#define BCK_PIN         18
#define WS_PIN          19
#define DATA_OUT_PIN    I2S_PIN_NO_CHANGE

void app_main(void)
{
    // I2S 드라이버 설정
    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,  // 마스터 모드, TX 전용
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS,
        .channel_format = I2S_CHANNEL_FMT,
        .communication_format = I2S_COMM_FMT,
        .intr_alloc_flags = 0,
        .dma_buf_count = 8,
        .dma_buf_len = 1024,
        .use_apll = false,
        .tx_desc_auto_clear = true, // 송신 DMA 버퍼가 비어도 자동으로 클리어
        .fixed_mclk = 0
    };

    // I2S 핀 설정: BCK와 WS에 외부 클럭 출력
    i2s_pin_config_t pin_config = {
        .bck_io_num = BCK_PIN,
        .ws_io_num = WS_PIN,
        .data_out_num = DATA_OUT_PIN,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    // I2S 드라이버 설치 및 핀 설정
    i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM, &pin_config);

    // 클럭을 계속 발생시키기 위해, 더미 데이터를 지속적으로 전송합니다.
    // 스테레오 16비트이므로 한 프레임은 4바이트입니다.
    // DMA 버퍼에 전송할 더미 데이터(0으로 채운 버퍼)를 준비합니다.
    const size_t buffer_size = 1024;
    uint8_t dummy_buffer[buffer_size];
    memset(dummy_buffer, 0, buffer_size);

    size_t bytes_written = 0;
    while (1) {
        // i2s_write를 통해 더미 데이터를 전송하면, I2S 하드웨어가
        // BCK와 WS 클럭 신호를 계속 생성합니다.
        i2s_write(I2S_NUM, dummy_buffer, buffer_size, &bytes_written, portMAX_DELAY);
        // 약간의 딜레이를 주어 CPU 사용률을 낮춥니다.
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
