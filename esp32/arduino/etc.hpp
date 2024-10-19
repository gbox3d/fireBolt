#include <WiFi.h>

String getChipID()
{
    uint8_t mac[6];
    WiFi.macAddress(mac);
    String macStr = "";
    for (int i = 0; i < 6; i++)
    {
        if (mac[i] < 0x10)
            macStr += "0"; // 한 자리 숫자 앞에 0 추가
        macStr += String(mac[i], HEX);
    }
    macStr.toUpperCase();
    return macStr;
}

