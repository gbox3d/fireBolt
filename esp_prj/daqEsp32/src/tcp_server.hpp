// tcp_server.hpp
#pragma once

#include <Arduino.h>
#include <AsyncTCP.h>
// #include <TaskScheduler.h>
#include "sampling_module.hpp"

#define MAX_BUFFER_SIZE 4096

class TcpServer {
public:
    TcpServer(uint16_t port, SamplingModule* sampler, const int *pLedPins);
    ~TcpServer();

    void begin();
    void sendData();

    void sendDataByEvent();

    void setBufferLimit(int limit) {
        
        if(limit > MAX_BUFFER_SIZE) limit = MAX_BUFFER_SIZE;

        bufferLimit = limit;
    }

    
private:
    AsyncServer* server;
    AsyncClient* client;
    uint16_t serverPort;
    bool isClientConnected;
    SamplingModule* samplingModule;

    const int *pLedPins;

    int m_bufferFSM;
    uint8_t m_buffer[MAX_BUFFER_SIZE];
    int bufferIndex;
    int bufferLimit;

    // Scheduler *m_pTaskMng;


    void handleNewClient(AsyncClient* newClient);
    void handleData(void* data, size_t len);
    void handleDisconnect();
};