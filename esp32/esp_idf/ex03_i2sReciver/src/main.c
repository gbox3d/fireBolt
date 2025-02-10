#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rmt_rx.h"     // 새로운 RMT RX API 사용
#include "driver/rmt_types.h"  // rmt_item32_t 타입 정의
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/ringbuf.h"

// 로그 출력 태그
static const char *TAG = "RMT_SAMPLE_NEW";

// 채널 핸들은 새 API에서는 rmt_channel_handle_t 타입을 사용합니다.
static rmt_channel_handle_t rx_channel_gpio32 = NULL;
static rmt_channel_handle_t rx_channel_gpio33 = NULL;

// RMT 수신 태스크: 해당 채널의 ring buffer에서 데이터를 읽어 출력
void rmt_rx_task(void *arg)
{
    rmt_channel_handle_t rx_channel = (rmt_channel_handle_t)arg;
    RingbufHandle_t rb = NULL;
    size_t rx_size = 0;
    rmt_item32_t *items = NULL;

    // 새 API의 rmt_get_ringbuf_handle() 사용
    ESP_ERROR_CHECK(rmt_get_ringbuf_handle(rx_channel, &rb));
    if (!rb) {
        ESP_LOGE(TAG, "Failed to get ring buffer handle");
        vTaskDelete(NULL);
    }

    while (1) {
        items = (rmt_item32_t *)xRingbufferReceive(rb, &rx_size, pdMS_TO_TICKS(1000));
        if (items) {
            int num_items = rx_size / sizeof(rmt_item32_t);
            // rx_channel는 포인터 타입이므로 %p에 전달할 때 (void *) 캐스팅
            ESP_LOGI(TAG, "Channel: %p, Items: %d", (void *)rx_channel, num_items);
            for (int i = 0; i < num_items; i++) {
                ESP_LOGI(TAG, "Item[%d]: dur0=%d, level0=%d, dur1=%d, level1=%d",
                         i,
                         items[i].duration0, items[i].level0,
                         items[i].duration1, items[i].level1);
            }
            vRingbufferReturnItem(rb, (void *)items);
        }
    }
    vTaskDelete(NULL);
}

void app_main(void)
{
    // 새로운 RMT RX 채널 설정 구조체 (rmt_rx_channel_config_t)
    rmt_rx_channel_config_t rx_channel_config = {
        .gpio_num = GPIO_NUM_32,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000,       // 1 MHz 해상도 → 1µs tick
        .mem_block_symbols = 64,
        .flags.invert_in = false,
        .flags.with_dma = false,
        .intr_priority = 0,
        .allow_pd = 0,
    };

    // GPIO32에 대해 새 RMT RX 채널 생성 및 시작
    ESP_ERROR_CHECK(rmt_new_rx_channel(&rx_channel_config, &rx_channel_gpio32));
    ESP_ERROR_CHECK(rmt_rx_start(rx_channel_gpio32, true));

    // GPIO33 채널: 설정 복사 후 gpio_num만 변경
    rx_channel_config.gpio_num = GPIO_NUM_33;
    ESP_ERROR_CHECK(rmt_new_rx_channel(&rx_channel_config, &rx_channel_gpio33));
    ESP_ERROR_CHECK(rmt_rx_start(rx_channel_gpio33, true));

    // 각각의 채널에 대해 수신 태스크 생성
    xTaskCreate(rmt_rx_task, "rmt_rx_task_gpio32", 2048, (void *)rx_channel_gpio32, 10, NULL);
    xTaskCreate(rmt_rx_task, "rmt_rx_task_gpio33", 2048, (void *)rx_channel_gpio33, 10, NULL);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
