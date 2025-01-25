#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

// 빌트인 LED 핀 번호 (LOLIN D32의 경우 GPIO5)
#define LED_GPIO GPIO_NUM_5

void app_main(void)
{
    // GPIO 핀을 출력 모드로 설정
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    printf("Hello, World!\n");

    while (1)
    {
        // LED 켜기
        gpio_set_level(LED_GPIO, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS); // 500ms 대기
        printf("LED ON\n");
        

        // LED 끄기
        gpio_set_level(LED_GPIO, 0);
        vTaskDelay(500 / portTICK_PERIOD_MS); // 500ms 대기
        printf("LED OFF\n");
    }
}
