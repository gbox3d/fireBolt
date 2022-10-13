#include <Arduino.h>
#include <FS.h>
#include <ArduinoJson.h>

class CespConfig
{
private:
    /* data */
public:
    CespConfig(/* args */);
    ~CespConfig();
    
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

    int load(File &file);
};

CespConfig::CespConfig(/* args */)
{
}

CespConfig::~CespConfig()
{
}

inline int CespConfig::load(File &configFile)
{
    DynamicJsonDocument jsonBuffer(2048);

    deserializeJson(jsonBuffer, configFile);

    this->version = jsonBuffer["version"];
    this->ssid = String((const char *)jsonBuffer["ssid"]);
    this->passwd = String((const char *)jsonBuffer["passwd"]);
    this->index = jsonBuffer["index"];
    this->remoteIp = String((const char *)jsonBuffer["remoteIp"]);
    this->remotePort = jsonBuffer["remotePort"];
    this->localIp = String((const char *)jsonBuffer["localIp"]);
    this->localPort = jsonBuffer["localPort"];
    this->loopDelay_req = jsonBuffer["loopDelay_req"];
    this->loopDelay_res = jsonBuffer["loopDelay_res"];
    this->trigger_delay = jsonBuffer["trigger_delay"];
    this->relay_pluse = jsonBuffer["relay_pluse"];
    this->max_fire_count = jsonBuffer["max_fire_count"];

    return 0;
}

