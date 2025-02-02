#include "driver/i2s.h"
#include "esp_log.h"
#include "soc/io_mux_reg.h"
#include "soc/i2s_reg.h"
#include "soc/gpio_sig_map.h"

#if (ESP_IDF_VERSION_MAJOR >= 5)
#define gpio_matrix_in(a, b, c) esp_rom_gpio_connect_in_signal(a, b, c)
#endif

#define I2S_NUM         I2S_NUM_0
#define DATA_PIN_BASE   25  // DATA0 핀 시작 번호
#define PCLK_PIN        22  // 픽셀 클럭 핀 번호
#define VSYNC_PIN       21  // 수직 동기화 핀 번호
#define HSYNC_PIN       19  // 수평 동기화 핀 번호

static const char *TAG = "I2S_Parallel_Input";

void app_main(void)
{
    // I2S 설정
    i2s_config_t i2s_config = {
        .mode = I2S_MODE_SLAVE | I2S_MODE_RX | I2S_MODE_PDM,
        .sample_rate = 1000000,  // 샘플링 속도 설정
        .bits_per_sample = I2S_BITS_PER_SAMPLE_8BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 1024,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0
    };

    // I2S 드라이버 설치
    ESP_ERROR_CHECK(i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL));

    // I2S 핀 설정
    i2s_pin_config_t pin_config = {
        .bck_io_num = PCLK_PIN,
        .ws_io_num = HSYNC_PIN,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = I2S_PIN_NO_CHANGE
    };
    ESP_ERROR_CHECK(i2s_set_pin(I2S_NUM, &pin_config));

    // 데이터 핀 매핑
    for (int i = 0; i < 8; i++) {
        gpio_set_direction(DATA_PIN_BASE + i, GPIO_MODE_INPUT);
        gpio_matrix_in(DATA_PIN_BASE + i, I2S0I_DATA_IN0_IDX + i, false);
    }

    // VSYNC 핀 설정
    gpio_set_direction(VSYNC_PIN, GPIO_MODE_INPUT);
    gpio_matrix_in(VSYNC_PIN, I2S0I_V_SYNC_IDX, false);

    printf("Hello, World!\n");

    // 데이터 수신
    size_t bytes_read;
    uint8_t data_buffer[1024];
    while (1) {
        i2s_read(I2S_NUM, data_buffer, sizeof(data_buffer), &bytes_read, portMAX_DELAY);
        ESP_LOGI(TAG, "Data received: %d bytes", bytes_read);
        // 수신된 데이터 처리
        if(bytes_read > 0) {
            for (int i = 0; i < bytes_read; i++) {
                // ESP_LOGI(TAG, "Data[%d]: %d", i, data_buffer[i]);
                if(data_buffer[i] != 0x00) {
                    ESP_LOGI(TAG, "VSYNC detected");
                }
            }
        }
    }

    
}
