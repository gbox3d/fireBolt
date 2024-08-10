// tcp_server.hpp
#pragma once

#include <Arduino.h>
#include <AsyncTCP.h>
// #include <TaskScheduler.h>
#include "sampling_module.hpp"

class TcpServer {
public:
    TcpServer(uint16_t port, SamplingModule* sampler, const int *pLedPins);
    ~TcpServer();

    void begin();
    void sendData();

    //callback functions pointer
    // void (*onTimeOut)(void);
    std::function<void()> onTimeOut;

private:
    AsyncServer* server;
    AsyncClient* client;
    uint16_t serverPort;
    bool isClientConnected;
    SamplingModule* samplingModule;

    const int *pLedPins;

    // Scheduler *m_pTaskMng;


    void handleNewClient(AsyncClient* newClient);
    void handleData(void* data, size_t len);
    void handleDisconnect();
};