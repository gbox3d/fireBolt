#include <Arduino.h>
#include <driver/i2s.h>

#define I2S_PORT I2S_NUM_0
#define GPIO_I2S_DATA_IN 27 // 디지털 신호 입력 핀
#define SAMPLE_RATE 100000  // 샘플링 속도 (100kHz)
#define DMA_BUF_LEN 1024    // DMA 버퍼 크기
#define DMA_BUF_COUNT 4     // DMA 버퍼 개수

void setup() {
  Serial.begin(115200);
  delay(1000); // 시리얼 모니터 초기화 대기

  // I2S 구성
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX), // I2S 마스터 및 RX 모드
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_8BIT, // 16비트 샘플
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, // 단일 채널
    .communication_format = I2S_COMM_FORMAT_STAND_I2S, // 최신 표준 I2S 프로토콜
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = DMA_BUF_COUNT,
    .dma_buf_len = DMA_BUF_LEN,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0,
  };

  // I2S 초기화
  if (i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL) != ESP_OK) {
    Serial.println("I2S Driver install failed");
    return;
  }

  // I2S 핀 설정
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_PIN_NO_CHANGE,      // 비트 클럭 사용 안 함
    .ws_io_num = I2S_PIN_NO_CHANGE,       // 워드 선택 사용 안 함
    .data_out_num = I2S_PIN_NO_CHANGE,    // 출력 핀 없음
    .data_in_num = GPIO_I2S_DATA_IN,      // 입력 데이터 핀
  };

  
  if (i2s_set_pin(I2S_PORT, &pin_config) != ESP_OK) {
    Serial.println("I2S Pin setup failed");
    return;
  }

  Serial.println("I2S initialized");
}

void loop() {
  // 데이터 버퍼
  uint16_t data_buffer[DMA_BUF_LEN];
  size_t bytes_read = 0;

  // I2S 데이터 읽기
  esp_err_t result = i2s_read(I2S_PORT, (void *)data_buffer, DMA_BUF_LEN * sizeof(uint16_t), &bytes_read, portMAX_DELAY);
  if (result != ESP_OK) {
    Serial.printf("I2S read error: %s\n", esp_err_to_name(result));
    return;
  }

  // 읽은 샘플 개수
  size_t num_samples = bytes_read / sizeof(uint16_t);

  // 신호 분석
  for (size_t i = 0; i < num_samples; i++) {
    if (data_buffer[i] > 1000) { // 임계값 설정
      Serial.printf("Signal detected! Value: %d, %d\n", data_buffer[i],i);
      break; // 신호를 검출하면 종료
    }
  }

  delay(10); // CPU 부하 방지
}
