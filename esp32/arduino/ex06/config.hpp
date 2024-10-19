#pragma once

struct S_Config_Data
{

    char ssid[16];
    char password[16];

    uint32_t nDeviceNumber;
    uint8_t nDevType;
    uint8_t reserved[27]; // pack 32 bytes
};

void writeConfig(const S_Config_Data &config);
bool readConfig(S_Config_Data &config);
void createDefaultConfig();
void dumpConfig(const S_Config_Data &config);

void dumpConfig(const S_Config_Data &config)
{
    Serial.println("Config:");
    Serial.println(config.ssid);
    Serial.println(config.password);
    Serial.println(config.nDeviceNumber);
    Serial.println(config.nDevType);
    // Serial.println(config.reserved[0]);
}

void writeConfig(const S_Config_Data &config)
{
    File file = SPIFFS.open("/config.dat", FILE_WRITE);
    if (!file)
    {
        Serial.println("Failed to open config file for writing");
        return;
    }
    file.write((uint8_t *)&config, sizeof(S_Config_Data));
    file.close();
    Serial.println("Config written");
}

bool readConfig(S_Config_Data &config)
{
    File file = SPIFFS.open("/config.dat", FILE_READ);
    if (!file)
    {
        Serial.println("Failed to open config file for reading");
        return false;
    }
    file.read((uint8_t *)&config, sizeof(S_Config_Data));
    file.close();
    Serial.println("Config read");
    return true;
}

void createDefaultConfig()
{
    if (SPIFFS.exists("/config.dat"))
    {
        Serial.println("Config file already exists");
        return;
    }

    S_Config_Data config;
    // 여기에 초기 설정 값을 채워 넣으세요.
    // 예를 들어:
    strncpy(config.ssid, "", sizeof(config.ssid));
    strncpy(config.password, "", sizeof(config.password));
    config.nDeviceNumber = 0;
    config.nDevType = 0;
    memset(config.reserved, 0, sizeof(config.reserved));

    writeConfig(config);
    Serial.println("Default config file created");
}