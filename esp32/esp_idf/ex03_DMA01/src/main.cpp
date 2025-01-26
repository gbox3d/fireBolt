#include <Arduino.h>
#include <driver/i2s.h>
#include <driver/gpio.h>
#include "soc/i2s_reg.h"
#include "soc/i2s_struct.h"

#define SAMPLE_RATE 100000
#define DMA_BUF_COUNT 8
#define DMA_BUF_LEN 256

#define DATA_PIN_0 26
#define DATA_PIN_1 27

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(DATA_PIN_0, INPUT);
  pinMode(DATA_PIN_1, INPUT);

  i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
      .sample_rate = SAMPLE_RATE,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_8BIT,
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = DMA_BUF_COUNT,
      .dma_buf_len = DMA_BUF_LEN,
      .use_apll = false,
  };

  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);

  i2s_pin_config_t pin_config = {
      .bck_io_num = 14,
      .ws_io_num = 15,
      .data_out_num = -1,
      .data_in_num = -1
  };
  i2s_set_pin(I2S_NUM_0, &pin_config);

  gpio_matrix_in(DATA_PIN_0, I2S0I_DATA_IN0_IDX, false);
  gpio_matrix_in(DATA_PIN_1, I2S0I_DATA_IN1_IDX, false);

  i2s_set_clk(I2S_NUM_0, SAMPLE_RATE, I2S_BITS_PER_SAMPLE_8BIT, I2S_CHANNEL_MONO);

  // LCD 병렬 모드 활성화
  I2S0.conf2.lcd_en = 1;
  I2S0.conf.rx_right_first = 1;

  Serial.println("I2S configuration complete.");
}

void loop() {
  uint8_t dma_buffer[DMA_BUF_LEN];
  size_t bytes_read = 0;

  if (i2s_read(I2S_NUM_0, dma_buffer, DMA_BUF_LEN, &bytes_read, portMAX_DELAY) == ESP_OK) {

    Serial.printf("Read %u bytes:\n", bytes_read);
    for (size_t i = 0; i < bytes_read; i++) {
      uint8_t data = dma_buffer[i];

      if (data == 0x00) {
        continue;
      }
      uint8_t bit0 = data & 0x1;
      uint8_t bit1 = (data >> 1) & 0x1;
      Serial.printf("Sample[%d]: 0x%02X (bit0=%d, bit1=%d)\n", i, data, bit0, bit1);
    }
  } else {
    Serial.println("Failed to read DMA buffer.");
  }

  delay(10);
}