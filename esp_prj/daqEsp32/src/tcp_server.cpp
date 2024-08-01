// tcp_server.cpp
#include "tcp_server.hpp"
#include "packet.hpp"

TcpServer::TcpServer(uint16_t port, SamplingModule* sampler)
    : serverPort(port), isClientConnected(false), samplingModule(sampler) {
    server = new AsyncServer(serverPort);
}

TcpServer::~TcpServer() {
    if (server) {
        delete server;
    }
    if (client) {
        client->close();
    }
}

void TcpServer::begin() {
    server->onClient([this](void* arg, AsyncClient* c) {
        this->handleNewClient(c);
    }, nullptr);

    server->begin();
    Serial.println("TCP server started");
}

void TcpServer::handleNewClient(AsyncClient* newClient) {
    if (client) {
        newClient->close();
        return;
    }
    client = newClient;
    isClientConnected = true;
    Serial.println("Client connected");

    client->onData([this](void*, AsyncClient*, void* data, size_t len) {
        this->handleData(data, len);
    }, nullptr);

    client->onDisconnect([this](void*, AsyncClient*) {
        this->handleDisconnect();
    }, nullptr);

    client->onError([](void*, AsyncClient*, int8_t error) {
        Serial.println("Client connection error");
    }, nullptr);
}

void TcpServer::handleData(void* data, size_t len) {
    S_PACKET_HEADER* header = (S_PACKET_HEADER*)data;
    if (header->header != MAGIC_NUMBER) {
        Serial.println("Invalid magic number");
        client->close();
        return;
    }

    if (header->type == PacketType::REQ) {
        S_PACKET_REQ* req = (S_PACKET_REQ*)data;
        switch (req->cmd) {
            case Command::CMD_PING:
                {
                    S_PACKET_RES response;
                    makeHeaderPacket(&response.header, ESP.getEfuseMac(), PacketType::RES, sizeof(S_PACKET_RES));
                    response.result_code = 0;
                    client->write((const char*)&response, response.header.packet_size);
                }
                break;
            case Command::CMD_START_SAMPLING:
                samplingModule->startSampling();
                break;
            case Command::CMD_STOP_SAMPLING:
                samplingModule->stopSampling();
                break;
            default:
                break;
        }
    }
}

void TcpServer::handleDisconnect() {
    client = nullptr;
    isClientConnected = false;
    Serial.println("Client disconnected");
}

void TcpServer::sendData() {
    if (isClientConnected && samplingModule->isDataReady() && client) {
        uint32_t sequence = samplingModule->getSequenceNumber();
        const uint8_t* data = samplingModule->getData();
        size_t dataSize = samplingModule->getDataSize();

        uint32_t header[2] = {MAGIC_NUMBER, sequence};

        client->write((const char*)header, sizeof(header));
        client->write((const char*)data, dataSize);

        samplingModule->releaseData();
    }
}