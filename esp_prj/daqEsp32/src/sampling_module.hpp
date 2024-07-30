// #pragma once

#include <Arduino.h>

#ifdef ESP8266
#include <ESPAsyncUDP.h>
#elif ESP32
#include <AsyncUDP.h>
#endif

class SamplingModule {
private:
    struct SampleBuffer {
        uint8_t* data;
        uint32_t sequence;
        bool ready;
    };

    const int* MIC_PINS;
    int NUM_CHANNELS;
    int SAMPLE_RATE;
    int BUFFER_SIZE;
    int BUFFER_SIZE_BYTES;

    SampleBuffer* buffers;
    volatile int currentBuffer;
    volatile int writeIndex;
    volatile uint32_t sequenceCounter;

    hw_timer_t* timer;
    portMUX_TYPE timerMux;

    static SamplingModule* instance;

    static void IRAM_ATTR onTimer();

public:
    SamplingModule(const int* mic_pins, int num_channels, int sample_rate, int buffer_size);
    ~SamplingModule();

    void setup();
    void startSampling();
    void stopSampling();
    bool isDataReady();
    uint32_t getSequenceNumber();
    const uint8_t* getData();
    void releaseData();
    void sendUdpData(AsyncUDP& udp, const IPAddress& host, uint16_t port);
};
