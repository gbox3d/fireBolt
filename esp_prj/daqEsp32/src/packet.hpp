#ifndef PACKET_HPP
#define PACKET_HPP

#include <Arduino.h>


#define MAGIC_NUMBER 20240729


// 패킷 타입 정의
enum PacketType {
    REQ = 0,
    RES = 1,
    SYS = 2
};

// 명령어 정의
enum Command {
    CMD_PING = 0,
    CMD_START_SAMPLING = 1,
    CMD_STOP_SAMPLING = 2
};

#pragma pack(push, 1)

//16byte header
struct S_PACKET_HEADER  {
    uint32_t header;
    uint32_t chipId;
    uint8_t type; // 0: REQ, 1: RES, 2: SYS
    uint16_t packet_size;
    uint8_t extra[5];
};

// 20byte
struct S_PACKET_REQ {
    S_PACKET_HEADER header;
    uint8_t cmd;
    uint8_t param[3];
};

// 20byte
struct S_PACKET_RES {
    S_PACKET_HEADER header;
    uint8_t result_code;
    uint8_t extra[3];
};

#pragma pack(pop)


inline void makeHeaderPacket(S_PACKET_HEADER* header, uint32_t chipId, uint8_t type, uint16_t packet_size) {
    header->header = MAGIC_NUMBER;
    header->chipId = chipId;
    header->type = type;
    header->packet_size = packet_size;
    memset(header->extra, 0, sizeof(header->extra));
    
}

#endif