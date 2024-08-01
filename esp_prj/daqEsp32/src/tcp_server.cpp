// tcp_server.cpp
#include "tcp_server.hpp"
#include "packet.hpp"

TcpServer::TcpServer(uint16_t port, SamplingModule* sampler)
    : serverPort(port), isClientConnected(false), samplingModule(sampler) {
    server = new AsyncServer(serverPort);
    client = nullptr;
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
        Serial.println("Client already connected");
        return;
    }
    client = newClient;
    isClientConnected = true;
    Serial.println("Client connected");

    S_PACKET_RES response;
    makeHeaderPacket(&response.header, ESP.getEfuseMac(), PacketType::SYS, sizeof(S_PACKET_RES));
    response.result_code = 99;
    client->write((const char*)&response, response.header.packet_size);

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
        client = nullptr;
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
                    response.extra[0] = req->param[0];
                    client->write((const char*)&response, response.header.packet_size);
                }
                break;
            case Command::CMD_START_SAMPLING:
                samplingModule->startSampling();
                Serial.println("Start sampling");
                break;
            case Command::CMD_STOP_SAMPLING:
                samplingModule->stopSampling();
                Serial.println("Stop sampling");
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

        S_PACKET_DAQ _daqPacket;
        makeHeaderPacket(&_daqPacket.header, ESP.getEfuseMac(), PacketType::DAQ, sizeof(S_PACKET_DAQ));
        
        

        // makeHeaderPacket(&response.header, ESP.getEfuseMac(), PacketType::RES, sizeof(S_PACKET_RES));

        uint32_t sequence = samplingModule->getSequenceNumber();
        const uint8_t* data = samplingModule->getData();
        size_t dataSize = samplingModule->getDataSize();

        _daqPacket.sequence = sequence;
        _daqPacket.data_size = dataSize;

        client->write((const char*)&_daqPacket, sizeof(S_PACKET_DAQ));
        client->write((const char*)data, dataSize);

        // uint32_t header[2] = {MAGIC_NUMBER, sequence};

        // client->write((const char*)header, sizeof(header));
        // client->write((const char*)data, dataSize);
        Serial.print("sqeuence: ");
        Serial.print(sequence);
        Serial.print(" Data size: ");
        Serial.print(dataSize);
        Serial.println(" sent");

        samplingModule->releaseData();
    }
}