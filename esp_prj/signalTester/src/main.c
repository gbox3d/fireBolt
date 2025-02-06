#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h" // 추가: 큐 기능 사용을 위한 헤더 파일
#include "driver/gpio.h"
#include "esp_timer.h"

#define INPUT_PIN1 32
#define INPUT_PIN2 33
#define OUTPUT_PIN_1 26
#define OUTPUT_PIN_2 27
#define LED_PIN 5

// 이벤트 큐 핸들 선언
static QueueHandle_t gpio_evt_queue = NULL; // 수정: xQueueHandle -> QueueHandle_t

// ISR: 이벤트 큐에 입력 핀 번호 전송
static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    // 인터럽트 중지처리
    gpio_intr_disable((uint32_t)arg);
    uint32_t gpio_num = (uint32_t)arg;

    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

// 신호 처리 태스크
void signal_task(void *pvParameter)
{
    uint32_t io_num;
    for (;;)
    {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
        {

            printf("Input signal detected on GPIO %lu\n", io_num);

            gpio_set_level(LED_PIN, 0);
            vTaskDelay(pdMS_TO_TICKS(250));
            gpio_set_level(LED_PIN, 1);
            vTaskDelay(pdMS_TO_TICKS(250));

            switch (io_num)
            {
            case INPUT_PIN1:
                // 출력 신호 생성: 26번 핀에 100µs HIGH, 그 후 27번 핀에 100µs HIGH
                gpio_set_level(OUTPUT_PIN_1, 1);
                esp_rom_delay_us(100);
                gpio_set_level(OUTPUT_PIN_2, 1);

                esp_rom_delay_us(50);
                gpio_set_level(OUTPUT_PIN_1, 0);
                gpio_set_level(OUTPUT_PIN_2, 0);
                break;
            case INPUT_PIN2:
                // 출력 신호 생성: 27번 핀에 100µs HIGH, 그 후 26번 핀에 100µs HIGH
                gpio_set_level(OUTPUT_PIN_2, 1);
                esp_rom_delay_us(100);
                gpio_set_level(OUTPUT_PIN_1, 1);

                esp_rom_delay_us(50);
                gpio_set_level(OUTPUT_PIN_2, 0);
                gpio_set_level(OUTPUT_PIN_1, 0);

                break;
            }

            vTaskDelay(pdMS_TO_TICKS(500));

            // 인터럽트 재개처리
            gpio_intr_enable(io_num);
        }
    }
}

void app_main(void)
{
    gpio_config_t io_conf;

    // 이벤트 큐 생성
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    // 입력 핀 설정 32
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << INPUT_PIN1) | (1ULL << INPUT_PIN2);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    // 출력 핀 설정
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << OUTPUT_PIN_1) | (1ULL << OUTPUT_PIN_2) | (1ULL << LED_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    // ISR 서비스 설치 및 핸들러 추가
    gpio_install_isr_service(0);
    gpio_isr_handler_add(INPUT_PIN1, gpio_isr_handler, (void *)INPUT_PIN1);
    gpio_isr_handler_add(INPUT_PIN2, gpio_isr_handler, (void *)INPUT_PIN2);

    // 신호 처리 태스크 생성
    xTaskCreate(signal_task, "signal_task", 2048, NULL, 10, NULL);

    gpio_set_level(OUTPUT_PIN_1, 0);
    gpio_set_level(OUTPUT_PIN_2, 0);
    gpio_set_level(LED_PIN, 1);

    printf("Signal processing task is running\n");

    // 메인 태스크 대기
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        // printf("Main task is running\n");
    }
}
