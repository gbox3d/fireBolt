#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include "SPIFFS.h"

#include "config.hpp"
#include "ble_packet_v1.hpp"


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

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;

// UUID for service and characteristic
#define SERVICE_UUID "02cb14bf-42e0-41bd-9d6d-68db02057e42"
#define CHARACTERISTIC_UUID "fef6efb5-fbf8-4d0a-a69f-57049733a87a"
#define LED_BUILTIN 5 // GPIO 핀 번호에 따라 변경할 수 있습니다.

#define CHECK_CODE 230815

int ledPins[] = {18, 19, 21, 22, 23, 13}; // 사용할 LED 핀 번호

S_Config_Data g_config;

class MyCharateristicCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        Serial.println("Characteristic write event");
        std::string value = pCharacteristic->getValue();

        if (value.length() >= sizeof(S_Ble_Header_Req_Packet_V1))
        {
            S_Ble_Header_Req_Packet_V1 *packet = (S_Ble_Header_Req_Packet_V1 *)value.data();

            if (packet->checkCode == CHECK_CODE)
            {
                switch (packet->cmd)
                {
                case 0x01:
                {
                    S_Ble_Header_Res_Packet_V1 resPacket;
                    resPacket.checkCode = CHECK_CODE;
                    resPacket.cmd = 0x01;
                    resPacket.parm[0] = packet->parm[0];

                    Serial.println("Echoing first parameter:");
                    Serial.println(packet->parm[0], HEX);

                    pCharacteristic->setValue((uint8_t *)&resPacket, sizeof(resPacket));
                }

                break;
                case 0x02:
                {
                    int nPinIndex = packet->parm[0];

                    digitalWrite(ledPins[nPinIndex], HIGH);
                }

                    // for (int i = 0; i < sizeof(ledPins) / sizeof(ledPins[0]); i++)
                    // {
                    //     digitalWrite(ledPins[i], HIGH);
                    // }
                    Serial.printf("LEDs ON: %d\n", packet->parm[0]);
                    break;
                case 0x03:
                {
                    int nPinIndex = packet->parm[0];

                    digitalWrite(ledPins[nPinIndex], LOW);
                }
                    Serial.printf("LEDs OFF: %d\n", packet->parm[0]);

                    break;
                case 0x04:
                {
                    S_Ble_Header_Res_Packet_V1 resPacket;
                    resPacket.checkCode = CHECK_CODE;
                    resPacket.cmd = 0x04;
                    resPacket.parm[0] = 1;
                    resPacket.parm[1] = 2;
                    resPacket.parm[2] = 3;
                    // notify the client of the new value
                    pCharacteristic->setValue((uint8_t *)&resPacket, sizeof(resPacket));
                    // delay(500);
                    pCharacteristic->notify();
                    break;
                }
                case 0x05: // 예를 들어, 0x05 명령어를 설정 데이터 저장으로 사용
                {
                    S_Config_Data_Req_Packet *configPacket = (S_Config_Data_Req_Packet *)value.data();
                    writeConfig(configPacket->data);
                }
                break;
                case 0x06: // 예를 들어, 0x06 명령어를 설정 데이터 읽기로 사용
                {
                    S_Config_Data config;
                    if (readConfig(config))
                    {
                        dumpConfig(config);
                        //send config data
                        S_Ble_Header_Res_Packet_V1 resPacket;
                        resPacket.checkCode = CHECK_CODE;
                        resPacket.cmd = 0x06;
                        S_Config_Data_Res_Packet resConfigPacket;
                        resConfigPacket.header = resPacket;
                        resConfigPacket.data = config;
                        pCharacteristic->setValue((uint8_t *)&resConfigPacket, sizeof(resConfigPacket));
                        // pCharacteristic->notify();
                    }
                }
                break;
                default:
                    Serial.println("Unknown command");
                }
            }
            else
            {
                Serial.println("Invalid check code");
            }
        }
        else
        {
            Serial.println("Received value too short");
        }
    }
};

class MyServerCallbacks : public BLEServerCallbacks
{

    void onConnect(BLEServer *pServer)
    {
        deviceConnected = true;
        digitalWrite(LED_BUILTIN, HIGH);
        // digitalWrite(pins[0], HIGH);
        Serial.println("Client connected");
        pServer->getAdvertising()->stop(); // 클라이언트가 연결되면 광고 중지
        // 환영 메시지 설정 및 알림 보내기
        {
            S_Ble_Header_Res_Packet_V1 resPacket;
            resPacket.checkCode = CHECK_CODE;
            resPacket.cmd = 0x99;
            pCharacteristic->setValue((uint8_t *)&resPacket, sizeof(resPacket));
            pCharacteristic->notify();
        }
    };

    void onDisconnect(BLEServer *pServer)
    {
        deviceConnected = false;
        digitalWrite(LED_BUILTIN, LOW);
        // digitalWrite(pins[0], LOW);
        Serial.println("Client disconnected");
        pServer->getAdvertising()->start(); // 클라이언트가 연결 해제되면 광고 다시 시작
    }
};


void setup()
{
    Serial.begin(115200);

    pinMode(LED_BUILTIN, OUTPUT); // 내장 LED를 출력 모드로 설정
    digitalWrite(LED_BUILTIN, HIGH);

    delay(300);

    if (!SPIFFS.begin(true))
    { // true는 SPIFFS 포맷이 필요한 경우 자동으로 수행하도록 함
        Serial.println("SPIFFS Mount Failed");
        return;
    }
    Serial.println("SPIFFS Mount Successful");

    //config 정보를 읽어온다.
    createDefaultConfig();

    readConfig(g_config);
    dumpConfig(g_config);

    digitalWrite(LED_BUILTIN, LOW);

    for (int i = 0; i < sizeof(ledPins) / sizeof(ledPins[0]); i++)
    {
        pinMode(ledPins[i], OUTPUT); // LED 핀을 출력 모드로 설정
        digitalWrite(ledPins[i], LOW);
    }

    // Create the BLE Device
    BLEDevice::init("ESP32_BLE" + std::string(getChipID().c_str()));

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // Create the BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Create a BLE Characteristic
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_INDICATE);

    pCharacteristic->addDescriptor(new BLE2902()); // 알람 표시 기능활성화
    pCharacteristic->setCallbacks(new MyCharateristicCallbacks());

    // Start the service
    pService->start();

    // Start advertising
    pServer->getAdvertising()->start();
    Serial.printf("Waiting a client connection to notify...%s \n", getChipID().c_str());
}

void loop()
{
}
