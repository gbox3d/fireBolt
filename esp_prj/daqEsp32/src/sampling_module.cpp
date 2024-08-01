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

    server = nullptr;
    client = nullptr;

}

SamplingModule::~SamplingModule() {
    for (int i = 0; i < 2; i++) {
        delete[] buffers[i].data;
    }
    delete[] buffers;

    if (server) {
        delete server;
    }
    if (client) {
        client->close();
    }
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

// void SamplingModule::sendUdpData(AsyncUDP& udp, const IPAddress& host, uint16_t port) {
//     if (isDataReady()) {
//         uint32_t sequence = getSequenceNumber();
//         const uint8_t* data = getData();
        
//         uint32_t header[2] = {MAGIC_NUMBER, sequence};
        
//         udp.writeTo((uint8_t*)header, sizeof(header), host, port);
//         udp.writeTo(data, BUFFER_SIZE_BYTES, host, port);
        
//         releaseData();
//     }
// }

void SamplingModule::setupTCPServer(uint16_t port) {
    serverPort = port;
    server = new AsyncServer(serverPort);

    server->onClient([](void* obj, AsyncClient* c) {
        SamplingModule* module = (SamplingModule*)obj;
        if (module->client) {
            // 이미 클라이언트가 연결되어 있으면 새 연결을 거부
            c->close();
            return;
        }
        module->client = c;
        module->isClientConnected = true;
        Serial.println("Client connected");

        c->onData([](void* obj, AsyncClient* c, void* data, size_t len) {
            
            SamplingModule* module = (SamplingModule*)obj;

            S_PACKET_HEADER* header = (S_PACKET_HEADER*)data;
            if (header->header != MAGIC_NUMBER) {
                Serial.println("Invalid magic number");
                c->close();
                return;
            }

            if (header->type == PacketType::REQ) {
                S_PACKET_REQ* req = (S_PACKET_REQ*)data;
                switch (req->cmd) {
                    case Command::CMD_PING:
                        // Serial.println("Ping received");
                        {
                            S_PACKET_RES response;

                            makeHeaderPacket(&response.header, ESP.getEfuseMac(), PacketType::RES, 
                                sizeof(S_PACKET_RES)
                            );
                            response.result_code = 0;
                            // response.header.header = MAGIC_NUMBER;
                            // response.header.chipId = ESP.getEfuseMac(); // ESP32의 고유 ID
                            // response.header.type = PacketType::RES;
                            // response.header.packet_size = sizeof(S_PACKET_RES);
                            // response.result_code = 0;
                            c->write((const char*)&response, response.header.packet_size );
                        }
                        break;
                    case Command::CMD_START_SAMPLING:
                        module->startSampling();
                        break;
                    case Command::CMD_STOP_SAMPLING:
                        module->stopSampling();
                        break;
                    default:
                        break;
                }
            }
        },module);

        c->onDisconnect([](void* obj, AsyncClient* c) {
            SamplingModule* module = (SamplingModule*)obj;
            module->client = nullptr;
            module->isClientConnected = false;
            Serial.println("Client disconnected");
        }, module);

        c->onError([](void* obj, AsyncClient* c, int8_t error) {
            Serial.println("Client connection error");
        }, module);

    }, this);

    server->begin();
    Serial.println("TCP server started");
}

void SamplingModule::sendTcpData() {
    if (isClientConnected && isDataReady() && client) {
        uint32_t sequence = getSequenceNumber();
        const uint8_t* data = getData();

        // 헤더 준비 (매직 넘버와 시퀀스 번호)
        uint32_t header[2] = {MAGIC_NUMBER, sequence};

        // 헤더 전송
        client->write((const char*)header, sizeof(header));

        // 데이터 전송
        client->write((const char*)data, BUFFER_SIZE_BYTES);

        releaseData();
    }
}

