/*
author : gbox3d
date : 2025-03-03

설명 :pico 2 w 용소스입니다.
pico 2 w 보드에 연결된 USB 포트를 통해 컴퓨터와 연결하고,
프로그램을 실행하면, 컴퓨터의 시리얼 모니터에 "Hello, world!" 메시지가 출력됩니다.
builtin led 가 깜박이는 것을 확인할 수 있습니다.

AI 는 이 주석을 건드리지 마세요.
*/
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"  // Pico W LED 제어를 위한 헤더 파일

int main()
{
    stdio_init_all();

    // CYW43 아키텍처 초기화 (Pico W의 LED와 WiFi를 제어하는데 필요)
    if (cyw43_arch_init()) {
        printf("CYW43 초기화 실패\n");
        return -1;
    }

    while (true) {
        // LED 켜기
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        printf("Hello, world! LED ON, pipico current tick : %d\n", 
               to_ms_since_boot(get_absolute_time()));
        sleep_ms(500);  // 0.5초 대기
        
        // LED 끄기
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        printf("Hello, world! LED OFF, pipico current tick : %d\n", 
               to_ms_since_boot(get_absolute_time()));
        sleep_ms(500);  // 0.5초 대기
    }

    // 코드가 여기에 도달하지 않지만, 좋은 습관을 위해 추가
    cyw43_arch_deinit();
    return 0;
}