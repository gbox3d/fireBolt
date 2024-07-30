#ifdef ESP8266
#include <ESPAsyncUDP.h>
#elif ESP32
#include <AsyncUDP.h>
#endif

#include "sampling_module.hpp"
#include "packet.hpp"


SamplingModule* SamplingModule::instance = nullptr;

SamplingModule::SamplingModule(const int* mic_pins, int num_channels, int sample_rate, int buffer_size)
    : MIC_PINS(mic_pins), NUM_CHANNELS(num_channels), SAMPLE_RATE(sample_rate), BUFFER_SIZE(buffer_size) {
    BUFFER_SIZE_BYTES = (BUFFER_SIZE * NUM_CHANNELS + 7) / 8;
    buffers = new SampleBuffer[2];
    for (int i = 0; i < 2; i++) {
        buffers[i].data = new uint8_t[BUFFER_SIZE_BYTES];
        buffers[i].ready = false;
    }
    currentBuffer = 0;
    writeIndex = 0;
    sequenceCounter = 0;
    timerMux = portMUX_INITIALIZER_UNLOCKED;
    instance = this;
}

SamplingModule::~SamplingModule() {
    for (int i = 0; i < 2; i++) {
        delete[] buffers[i].data;
    }
    delete[] buffers;
}

void IRAM_ATTR SamplingModule::onTimer() {
    if (!instance) return;

    portENTER_CRITICAL_ISR(&instance->timerMux);
    
    uint8_t sample = 0;
    for (int i = 0; i < instance->NUM_CHANNELS; i++) {
        sample |= (digitalRead(instance->MIC_PINS[i]) << i);
    }
    
    int byteIndex = instance->writeIndex / 8;
    int bitIndex = instance->writeIndex % 8;
    
    if (sample) {
        instance->buffers[instance->currentBuffer].data[byteIndex] |= (1 << bitIndex);
    } else {
        instance->buffers[instance->currentBuffer].data[byteIndex] &= ~(1 << bitIndex);
    }
    
    instance->writeIndex++;
    
    if (instance->writeIndex >= instance->BUFFER_SIZE * instance->NUM_CHANNELS) {
        instance->buffers[instance->currentBuffer].sequence = instance->sequenceCounter++;
        instance->buffers[instance->currentBuffer].ready = true;
        instance->currentBuffer = 1 - instance->currentBuffer;
        instance->writeIndex = 0;
    }
    
    portEXIT_CRITICAL_ISR(&instance->timerMux);
}

void SamplingModule::setup() {
    for (int i = 0; i < NUM_CHANNELS; i++) {
        pinMode(MIC_PINS[i], INPUT);
    }
    
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, 1000000 / SAMPLE_RATE, true);
}

void SamplingModule::startSampling() {
    timerAlarmEnable(timer);
}

void SamplingModule::stopSampling() {
    timerAlarmDisable(timer);
}

bool SamplingModule::isDataReady() {
    return buffers[0].ready || buffers[1].ready;
}

uint32_t SamplingModule::getSequenceNumber() {
    for (int i = 0; i < 2; i++) {
        if (buffers[i].ready) {
            return buffers[i].sequence;
        }
    }
    return 0;
}

const uint8_t* SamplingModule::getData() {
    for (int i = 0; i < 2; i++) {
        if (buffers[i].ready) {
            return buffers[i].data;
        }
    }
    return nullptr;
}

void SamplingModule::releaseData() {
    for (int i = 0; i < 2; i++) {
        if (buffers[i].ready) {
            buffers[i].ready = false;
            break;
        }
    }
}

void SamplingModule::sendUdpData(AsyncUDP& udp, const IPAddress& host, uint16_t port) {
    if (isDataReady()) {
        uint32_t sequence = getSequenceNumber();
        const uint8_t* data = getData();
        
        uint32_t header[2] = {MAGIC_NUMBER, sequence};
        
        udp.writeTo((uint8_t*)header, sizeof(header), host, port);
        udp.writeTo(data, BUFFER_SIZE_BYTES, host, port);
        
        releaseData();
    }
}