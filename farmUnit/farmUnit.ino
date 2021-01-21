#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <FS.h>
#include <WiFiUdp.h>
#include <Ticker.h>
// #include "gpioMng.hpp"

#include "packet.hpp"
#include "esfos_drv.hpp"

const byte version = 10;
const byte app_rev = 1;
const u32 system_Header = 20200519;
const int sensor_pin = D5;
const int led_pin = D1; //replay control
const int status_pin = D0;

u16 g_nLedPin_count = 0;
// String g_Pad_ip = "";
// uint16_t g_Pad_port = 0;

int nFSM = 0;
u32 g_chipId;
volatile boolean enableTriger = true;
boolean connectioWasAlive = false;

WiFiUDP Udp;

struct _S_CONFIG
{
    int version;
    String ssid;
    String passwd;
    int index; //사로 번호

    String remoteIp;
    int remotePort;

    String localIp;
    int localPort;

    u32 loopDelay_req;
    u32 loopDelay_res;

    u32 trigger_delay;
    u32 relay_pluse;

    int max_fire_count;
};

_S_CONFIG configObject;

void loadConfig(DynamicJsonDocument &doc)
{
    configObject.version = doc["version"];
    configObject.ssid = String((const char *)doc["ssid"]);
    configObject.passwd = String((const char *)doc["passwd"]);
    configObject.index = doc["index"];

    configObject.remoteIp = (const char *)doc["remoteIp"];
    configObject.remotePort = doc["remotePort"];

    configObject.localIp = (const char *)doc["localIp"];
    configObject.localPort = doc["localPort"];

    configObject.loopDelay_res = doc["loopDelay_res"];
    configObject.loopDelay_req = doc["loopDelay_req"];

    configObject.trigger_delay = doc["trigger_delay"];
    configObject.relay_pluse = doc["relay_pluse"];

    configObject.max_fire_count = doc["max_fire_count"];
}

void outConfig(DynamicJsonDocument &doc)
{
    //File f = SPIFFS.open(configFileName, "w");
    // DynamicJsonDocument doc(1024);

    doc["version"] = configObject.version;
    doc["ssid"] = configObject.ssid;
    doc["passwd"] = configObject.passwd;
    doc["index"] = configObject.index;

    doc["localIp"] = configObject.localIp;
    doc["localPort"] = configObject.localPort;

    doc["remoteIp"] = configObject.remoteIp;
    doc["remotePort"] = configObject.remotePort;

    doc["loopDelay_res"] = configObject.loopDelay_res;
    doc["loopDelay_req"] = configObject.loopDelay_req;

    doc["trigger_delay"] = configObject.trigger_delay;
    doc["relay_pluse"] = configObject.relay_pluse;
    doc["max_fire_count"] = configObject.max_fire_count;

    // serializeJson(doc, pStream);
}

const char *configFileName = "config.json";

void _format()
{
    Serial.println("format:");
    //Format File System
    if (SPIFFS.format())
    {
        Serial.println("File System Formated");
    }
    else
    {
        Serial.println("File System Formatting Error");
    }
}

void _sendData(byte *packet, u32 size)
{
    // Serial.printf("send to : %s , %d \n",configObject.remoteIp.c_str(), configObject.remotePort);
    Udp.beginPacket(configObject.remoteIp.c_str(), configObject.remotePort);
    Udp.write(packet, size);
    Udp.endPacket();
}

void udp_loop_req()
{
    // Serial.println("req");
    //요청
    S_REQ_PACKET _req;
    //memset(&_req, 0, sizeof(S_REQ_PACKET));
    _req.header = system_Header;
    _req.chipId = g_chipId;
    _req.index = configObject.index;
    _req.code = 0x01;
    _req.status = nFSM;
    _req.pkt_size = sizeof(S_REQ_PACKET);
    // _req.count = g_nFire_count;
    _req.count = g_nLedPin_count;
    _req.extra[0] = version;
    _req.extra[1] = app_rev;

    _sendData((byte *)&_req, sizeof(S_REQ_PACKET));

    nFSM = 0;
}

static byte packetBuffer[1024];

// void updateStatustoPad()
// {
//     //send status packet
//     if (g_Pad_ip != "")
//     {
//         //응답 팻킷
//         S_REQ_PACKET *_req = (S_REQ_PACKET *)packetBuffer;
//         _req->header = system_Header;
//         _req->chipId = g_chipId;
//         _req->index = configObject.index;
//         _req->code = 0xa6; //
//         _req->status = nFSM;
//         _req->pkt_size = sizeof(S_REQ_PACKET);
//         _req->count = g_nFire_count;
//         _req->extra[0] = version;

//         Udp.beginPacket(g_Pad_ip.c_str(), g_Pad_port);
//         Udp.write((byte *)_req, sizeof(S_REQ_PACKET));
//         Udp.endPacket();

//         // Serial.printf("send to %s %d\n",g_Pad_ip.c_str(), g_Pad_port);
//     }
// }

void udp_loop_res()
{
    // Serial.println("res");

    S_RES_PACKET *pResPkt;

    //응답처리
    int packetSize = Udp.parsePacket();
    if (packetSize)
    {
        // receive incoming UDP packets
        //  Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
        int len = Udp.read((byte *)packetBuffer, packetSize);

        pResPkt = (S_RES_PACKET *)packetBuffer;

        byte _param1 = pResPkt->param[0];
        byte _param2 = pResPkt->param[1];
        byte _code = pResPkt->code;

        Serial.printf("header : %d,code:%d\n",
            pResPkt->header,
            pResPkt->code);

        if (pResPkt->code == 0x11)
        { // ready to fire
            // g_nFire_count = *((u16 *)pResPkt->param);

            digitalWrite(led_pin, HIGH);
            g_nLedPin_count = 1;
            nFSM = 10;
            // updateStatustoPad();
        }
        else if (pResPkt->code == 0x12)
        { //stop file
            //enableTriger = false;
            digitalWrite(led_pin, LOW);
            g_nLedPin_count = 0;
            // digitalWrite(status_pin, HIGH);
            // enableTriger = false;
            // g_nFire_count = 0;
            nFSM = 0;
            // updateStatustoPad();
        }
        else if (pResPkt->code == 0x21) //read config
        {
            File f = SPIFFS.open(configFileName, "r");
            if (f)
            {
                String line = f.readString();
                f.close();
                Serial.println(line);
                Serial.printf("read %d bytes\n", line.length());

                u16 _size = line.length();
                {
                    S_REQ_PACKET *pPkt = (S_REQ_PACKET *)packetBuffer;
                    pPkt->header = system_Header;
                    pPkt->chipId = g_chipId;
                    pPkt->code = 0x21;
                    pPkt->status = nFSM;
                    pPkt->index = configObject.index;
                    pPkt->pkt_size = sizeof(S_REQ_PACKET);
                    pPkt->count = _size;

                    memcpy((char *)pPkt + sizeof(S_REQ_PACKET), line.c_str(), _size);

                    if (_param1 == 0x01)
                    {
                        Udp.beginPacket(Udp.remoteIP().toString().c_str(), Udp.remotePort());
                        Udp.write((byte *)packetBuffer, sizeof(S_REQ_PACKET) + _size);
                        Udp.endPacket();
                    }
                    else
                    {
                        _sendData((byte *)packetBuffer, sizeof(S_REQ_PACKET) + _size);
                    }
                }
            }
            else
            {
                Serial.printf("failed open %s \n", configFileName);
            }
        }
        else if (pResPkt->code == 0x22) //write config
        {
            char *ptr = ((char *)pResPkt) + sizeof(S_RES_PACKET);

            Serial.println(ptr);

            File f = SPIFFS.open(configFileName, "w");
            if (f)
            {
                u16 _size = f.write(ptr, packetSize - sizeof(S_RES_PACKET));

                Serial.printf("%d byte saved\n", _size);
                f.close();

                {
                    S_REQ_PACKET *pPkt = (S_REQ_PACKET *)packetBuffer;
                    pPkt->header = system_Header;
                    pPkt->chipId = g_chipId;
                    pPkt->code = 0x22;
                    pPkt->status = nFSM;
                    pPkt->index = configObject.index;
                    pPkt->pkt_size = sizeof(S_REQ_PACKET);
                    pPkt->count = _size;

                    if (_param1 == 1)
                    {
                        Udp.beginPacket(Udp.remoteIP().toString().c_str(), Udp.remotePort());
                        Udp.write((byte *)packetBuffer, sizeof(S_REQ_PACKET));
                        Udp.endPacket();
                    }
                    else
                    {
                        _sendData(packetBuffer, sizeof(S_REQ_PACKET));
                    }
                }
            }
            else
            {
                Serial.printf("failed open %s \n", configFileName);
            }
        }
        else if (pResPkt->code == 0x23) //format fifs
        {
            //format
            nFSM = 90;
            // SPIFFS.format();
            // ESP.restart();
        }
        else if (pResPkt->code == 0x05)
        { //req status
            S_REQ_PACKET *_req = (S_REQ_PACKET *)packetBuffer;
            //memset(&_req, 0, sizeof(S_REQ_PACKET));
            _req->header = system_Header;
            _req->chipId = g_chipId;
            _req->index = configObject.index;
            _req->code = 0x69; //json format packet
            _req->status = nFSM;
            _req->pkt_size = sizeof(S_REQ_PACKET);
            // _req->count = g_nFire_count;
            _req->count = 0;
            _req->extra[0] = version;

            DynamicJsonDocument doc(1024);
            doc["code"] = 0x05;
            doc["ip"] = WiFi.localIP().toString();

            String _jsonStr;
            serializeJson(doc, _jsonStr);

            // Serial.println(doc["ip"]);

            memcpy(packetBuffer + sizeof(S_REQ_PACKET), _jsonStr.c_str(), _jsonStr.length());

            Udp.beginPacket(Udp.remoteIP().toString().c_str(), Udp.remotePort());
            Udp.write((byte *)_req, sizeof(S_REQ_PACKET) + _jsonStr.length());
            Udp.endPacket();

            // _sendData((byte *)&_req, sizeof(S_REQ_PACKET));
        }
        // else if (pResPkt->code == 0x06)
        // { //req update from pad

        //     //페드주소 업데이트
        //     g_Pad_ip = Udp.remoteIP().toString();
        //     g_Pad_port = Udp.remotePort();

        //     //응답 팻킷
        //     updateStatustoPad();
        // }
        else if (pResPkt->code == 0x01)
        {
            //서버 핑에 대한 응답여기에는 응답하지않는다.
            //나중에 서버 지연시간 계산 코드 추가할것
            //Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
        }
        else if (pResPkt->code == 0x00)
        {
            ESP.restart();
        }
        else
        {
            Serial.printf("unknown code : %d \n", pResPkt->code);
        }
    }
}

Ticker g_tickerUdpReq;
Ticker g_tickerUdpRes;
// Ticker g_tickerCheckAp;

void newConfigFile()
{
    Serial.println("File doesn't exist yet. Creating it");

    DynamicJsonDocument doc(1024);
    //create new config file
    File f = SPIFFS.open(configFileName, "w");
    if (!f)
    {
        Serial.println("file creation failed");
    }
    else
    {
        doc["version"] = 2;
        doc["ssid"] = "redstar0427";
        doc["passwd"] = "123456789a";
        doc["index"] = 100;
        doc["localPort"] = 8284;
        doc["localIp"] = "";
        doc["remoteIp"] = "worweb.cafe24.com";
        doc["remotePort"] = 17395;
        doc["loopDelay_req"] = 1500;
        doc["loopDelay_res"] = 100;
        doc["trigger_delay"] = 150;
        doc["relay_pluse"] = 40;
        doc["max_fire_count"] = 16;

        serializeJson(doc, f);
        Serial.println("init config");
        loadConfig(doc);
    }
}

int _processConfigFile()
{ //file processing

    File f = SPIFFS.open(configFileName, "r");

    if (!f)
    { //파일이 없으면
        newConfigFile();
    }
    else
    {
        DynamicJsonDocument doc(1024);
        Serial.print("File found : " + String(configFileName));
        Serial.println(String(",file size : ") + (int)(f.size()));

        if (f.available())
        {
            String line = f.readString();
            Serial.println(line);
            if (deserializeJson(doc, line))
            {
                Serial.println("parse failed");
                return 2;
            }
            else
            {
                Serial.println("parse ok");
                Serial.println("version check");
                if (doc["version"] == 2)
                {
                    Serial.printf("ok \n");
                }
                else
                {
                    Serial.printf("failed : %d \n", (int)doc["version"]);
                    return 3;
                }
            }
        }
        else
        {
            Serial.println("empty file");
            return 1;
        }

        loadConfig(doc);
        f.close();
    }

    return 0;
}
////////

void setup()
{
    pinMode(led_pin, OUTPUT); //onoff 출력 제어(pwm 가능)
    digitalWrite(led_pin, LOW);

    pinMode(status_pin, OUTPUT); //onoff 출력 제어
    digitalWrite(status_pin, HIGH);

    pinMode(LED_BUILTIN, OUTPUT); // 상태표시 led
    digitalWrite(LED_BUILTIN, LOW);

    pinMode(sensor_pin, INPUT_PULLUP); //onoff 입력 센서 제어

    //칩아이디 얻기
    g_chipId = ESP.getChipId();

    Serial.begin(115200);

    delay(1000);
    Serial.printf("\n\nfarm Unit start:%d\n\n", version);

    bool result = SPIFFS.begin();
    if (result)
    {
        Serial.println("SPIFFS opened: ");

        int _ecode = _processConfigFile();
        if (_ecode != 0)
        {
            newConfigFile();
            ESP.restart();
            return;
        }
        else //setup success
        {
            digitalWrite(LED_BUILTIN, LOW); //AP 접속 대시 상태 표시
            delay(500);
            digitalWrite(LED_BUILTIN, HIGH); //AP 접속 대시 상태 표
            nFSM = 300; //station mode 접속시도 
        }
    }
    else
    {
        Serial.println("file system error");
        return;
    }
}

void loop()
{
    static u32 _wotkTick = 0;
    static bool _bvirtualStartBtnPush = false;

    //main control logic
    switch (nFSM)
    {
    case 0:
    {
        esfos::run_driver();
    }
    break;
    case 90: //포멧
    {
        nFSM = 91;
        Serial.println("formatting....");
        SPIFFS.format();
        Serial.println('format done..restart');
        delay(500);
        ESP.restart();
    }
    break;
    case 200: //리부트
    {
        delay(1000);
        ESP.restart();
    }
    break;
    case 300: //AP 접속 시도 , station mode
    {
        Serial.printf("wifi mode %d \n", WiFi.getMode());
        WiFi.mode(WIFI_STA);
        WiFi.begin(configObject.ssid, configObject.passwd);
        Serial.printf("%s,%s \n", configObject.ssid.c_str(), configObject.passwd.c_str());
        digitalWrite(LED_BUILTIN, HIGH);
        nFSM = 301;
        _wotkTick = millis();
    }
    break;
    case 301: //AP 접속시도
    {
        if (WiFi.status() == WL_CONNECTED) //접속 성공 하면...
        {
            digitalWrite(LED_BUILTIN, LOW);
            Serial.printf("\n\nConnected, IP address: %s \n\n", WiFi.localIP().toString().c_str());
            Serial.printf("Connected delay %d\n", millis() - _wotkTick);

            //udp ready
            Udp.begin(configObject.localPort);
            Serial.printf("Now listening at IP %s, UDP port %d\nRemote:%s:%d\n",
                          WiFi.localIP().toString().c_str(),
                          configObject.localPort,
                          configObject.remoteIp.c_str(),
                          configObject.remotePort);

            delay(500);
            // Serial.println("start req loop");
            g_tickerUdpReq.attach_ms(configObject.loopDelay_req, udp_loop_req);
            g_tickerUdpRes.attach_ms(configObject.loopDelay_res, udp_loop_res);
            // g_tickerUdpRes.attach_ms(100, udp_loop_res);
            connectioWasAlive = true;
            _wotkTick = millis();
            nFSM = 310;
        }
        else if ((millis() - _wotkTick) > 15000) //15응답없으면 재부팅
        {
            Serial.printf("AP Try timeout\n");
            delay(2000);
            ESP.restart();
        }
    }
    break;
    case 310:
    {
        if (abs(millis() - _wotkTick) > 500) //0.5초 대기
        {
            nFSM = 0;
            enableTriger = false; //센서 정지
        }
    }
    break;
    case 500: //off line mode , AP mode
    {
        WiFi.mode(WIFI_AP);
        WiFi.softAP(String("espap_") + g_chipId, "123456789");
        Udp.begin(configObject.localPort);

        delay(500);
        g_tickerUdpRes.attach_ms(configObject.loopDelay_res, udp_loop_res);

        Serial.printf("startup offline mode : %d , ", configObject.localPort);
        Serial.println(WiFi.softAPIP());

        nFSM = 0;
    }
    break;
    default:
        break;
    }
}
