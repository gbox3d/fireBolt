// tcp_server.hpp
#pragma once

#include <Arduino.h>
#include <AsyncTCP.h>
#include "sampling_module.hpp"

class TcpServer {
public:
    TcpServer(uint16_t port, SamplingModule* sampler);
    ~TcpServer();

    void begin();
    void sendData();

private:
    AsyncServer* server;
    AsyncClient* client;
    uint16_t serverPort;
    bool isClientConnected;
    SamplingModule* samplingModule;

    void handleNewClient(AsyncClient* newClient);
    void handleData(void* data, size_t len);
    void handleDisconnect();
};