// tcp_server.cpp
#include "tcp_server.hpp"
#include "packet.hpp"

TcpServer::TcpServer(uint16_t port, SamplingModule *sampler, const int *pLedPins)
    : serverPort(port), isClientConnected(false), samplingModule(sampler), pLedPins(pLedPins)
{
    server = new AsyncServer(serverPort);
    client = nullptr;
    // onTimeOut = nullptr;
    m_bufferFSM = 0;
    memset(m_buffer, 0, MAX_BUFFER_SIZE);
}

TcpServer::~TcpServer()
{
    if (server)
    {
        delete server;
    }
    if (client)
    {
        client->close();
    }
}

void TcpServer::begin()
{
    server->onClient([this](void *arg, AsyncClient *c)
                     { this->handleNewClient(c); }, nullptr);

    server->begin();
    Serial.println("TCP server started");
}

void TcpServer::handleNewClient(AsyncClient *newClient)
{
    if (client)
    {
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
    client->write((const char *)&response, response.header.packet_size);

    client->onData([this](void *, AsyncClient *, void *data, size_t len)
                   { this->handleData(data, len); }, nullptr);

    client->onDisconnect([this](void *, AsyncClient *)
                         { this->handleDisconnect(); }, nullptr);

    client->onError([](void *, AsyncClient *, int8_t error)
                    { Serial.println("Client connection error"); }, nullptr);
}

void TcpServer::handleData(void *data, size_t len)
{
    S_PACKET_HEADER *header = (S_PACKET_HEADER *)data;
    if (header->header != MAGIC_NUMBER)
    {
        Serial.println("Invalid magic number");
        client->close();
        client = nullptr;
        return;
    }

    if (header->type == PacketType::REQ)
    {
        S_PACKET_REQ *req = (S_PACKET_REQ *)data;
        switch (req->cmd)
        {
        case Command::CMD_PING:
        {
            S_PACKET_RES response;
            makeHeaderPacket(&response.header, ESP.getEfuseMac(), PacketType::RES, sizeof(S_PACKET_RES));
            response.result_code = 0;
            // Serial.println("ping response" + String(req->param[0]));
            response.extra[0] = req->param[0];
            client->write((const char *)&response, response.header.packet_size);
        }
        break;
        case Command::CMD_START_SAMPLING:
        {
            samplingModule->startSampling();

            int bufferSize = (req->param[0] << 8) | req->param[1];

            setBufferLimit(bufferSize);

            Serial.println("Start sampling");
            digitalWrite(pLedPins[0], HIGH);
        }

        break;
        case Command::CMD_STOP_SAMPLING:
            samplingModule->stopSampling();
            Serial.println("Stop sampling");
            digitalWrite(pLedPins[0], LOW);
            break;
        default:
            break;
        }
    }
}

void TcpServer::handleDisconnect()
{
    client = nullptr;
    isClientConnected = false;
    Serial.println("Client disconnected");
}

void TcpServer::sendData()
{
    if (isClientConnected && samplingModule->isDataReady() && client)
    {

        digitalWrite(pLedPins[0], HIGH);

        const uint32_t CHUNK_SIZE = 4096;

        S_PACKET_DAQ _daqPacket;
        uint32_t sequence = samplingModule->getSequenceNumber();
        const uint8_t *data = samplingModule->getData();
        size_t totalDataSize = samplingModule->getDataSize();

        // 헤더 패킷 생성 및 전송 (한 번만)
        makeHeaderPacket(&_daqPacket.header, ESP.getEfuseMac(), PacketType::DAQ, sizeof(S_PACKET_DAQ));
        _daqPacket.sequence = sequence;
        _daqPacket.data_size = totalDataSize;

        client->write((const char *)&_daqPacket, sizeof(S_PACKET_DAQ));

        // 데이터를 청크로 나누어 전송
        size_t remainingData = totalDataSize;
        size_t offset = 0;
        int totalSent = 0;

        while (remainingData > 0)
        {
            size_t chunkSize = (remainingData > CHUNK_SIZE) ? CHUNK_SIZE : remainingData;
            int datasent = client->write((const char *)(data + offset), chunkSize);

            totalSent += datasent;
            remainingData -= datasent;
            offset += datasent;

            // Serial.print("sequence: ");
            // Serial.print(sequence);
            // Serial.print(" Chunk sent: ");
            // Serial.print(datasent);
            // Serial.print(" Total sent: ");
            // Serial.print(totalSent);
            // Serial.print("/");
            // Serial.println(totalDataSize);

            // 각 청크 전송 후 약간의 지연을 줄 수 있습니다.
            // delay(50);
        }

        // Serial.print("Sequence ");
        // Serial.print(sequence);
        // Serial.print(" completed. Total data sent: ");
        // Serial.println(totalSent);

        digitalWrite(pLedPins[0], LOW);

        samplingModule->releaseData();
    }
}

void TcpServer::sendDataByEvent()
{
    if (bufferLimit == 0)
    {
        sendData();
    }
    else
    {

        if (isClientConnected && samplingModule->isDataReady() && client)
        {

            digitalWrite(pLedPins[0], HIGH);

            const uint32_t CHUNK_SIZE = 4096;

            S_PACKET_DAQ _daqPacket;
            uint32_t sequence = samplingModule->getSequenceNumber();
            const uint8_t *data = samplingModule->getData();
            size_t totalDataSize = samplingModule->getDataSize();

            switch (m_bufferFSM)
            {
            case 0: // standby
            {
                // data 중 0 이 아닌값이 있는지 확인
                //  Serial.println("Data check");
                int nonZeroIndex = -1;
                for (int i = 0; i < totalDataSize; i++)
                {
                    if (data[i] != 0)
                    {
                        nonZeroIndex = i;
                        break;
                    }
                }

                if (nonZeroIndex == -1)
                {
                    // Serial.println("Data is empty");
                    break;
                }
                else
                {
                    // add buffer
                    //  data size
                    int cpysize = totalDataSize - nonZeroIndex;
                    if (cpysize > bufferLimit)
                    {
                        cpysize = bufferLimit;
                    }

                    // memccpy(m_buffer, data, nonZeroIndex, cpysize); //copy data to buffer
                    memcpy(m_buffer, data + nonZeroIndex, cpysize);

                    bufferIndex = cpysize;

                    if (bufferIndex < bufferLimit)
                    {
                        m_bufferFSM = 10;
                    }
                    else
                    {

                        // 헤더 패킷 생성 및 전송 (한 번만)
                        makeHeaderPacket(&_daqPacket.header, ESP.getEfuseMac(), PacketType::DAQ, sizeof(S_PACKET_DAQ));
                        _daqPacket.sequence = sequence;
                        _daqPacket.data_size = bufferIndex;
                        client->write((const char *)&_daqPacket, sizeof(S_PACKET_DAQ));
                        client->write((const char *)m_buffer, _daqPacket.data_size);

                        // Serial.println("Buffer sent step 0");

                        bufferIndex = 0;
                    }
                }
            }
            break;
            case 10:
            {
                // 나머지 버퍼 체우기
                // 버퍼의 나머지 부분을 채워서 전송

                int remainingSpace = bufferLimit - bufferIndex;
                int cpysize = (totalDataSize < remainingSpace) ? totalDataSize : remainingSpace;

                memcpy(m_buffer + bufferIndex, data, cpysize);
                bufferIndex += cpysize;

                if (bufferIndex >= bufferLimit)
                {
                    makeHeaderPacket(&_daqPacket.header, ESP.getEfuseMac(), PacketType::DAQ, sizeof(S_PACKET_DAQ));
                    _daqPacket.sequence = sequence;
                    _daqPacket.data_size = bufferIndex;
                    client->write((const char *)&_daqPacket, sizeof(S_PACKET_DAQ));
                    client->write((const char *)m_buffer, bufferIndex);
                    bufferIndex = 0;
                    m_bufferFSM = 0;
                }
            }
            break;

            default:
                break;
            }

            digitalWrite(pLedPins[0], LOW);
            samplingModule->releaseData();
        }
    }
}