#pragma once

struct S_Ble_Header_Req_Packet_V1
{
    uint32_t checkCode;
    uint8_t cmd;
    uint8_t parm[3];
};

struct S_Ble_Header_Res_Packet_V1
{
    uint32_t checkCode;
    uint8_t cmd;
    uint8_t parm[3];
};

struct S_Config_Data_Req_Packet
{
    S_Ble_Header_Req_Packet_V1 header;
    S_Config_Data data;
};

struct S_Config_Data_Res_Packet
{
    S_Ble_Header_Res_Packet_V1 header;
    S_Config_Data data;
};