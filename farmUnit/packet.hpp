#ifndef PACKET_HPP
#define PACKET_HPP

struct S_REQ_PACKET  {
    u32 header;
    u32 chipId;
    byte code;
    byte status; 
    byte index;
    byte pkt_size; 
    u16 count;
    byte extra[2];
    
};

struct S_RES_PACKET {
    u32 header;
    byte code;
    // u16 count;
    byte param[2];
    byte extra;
};

#endif