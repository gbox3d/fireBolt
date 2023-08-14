#include <WiFi.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <etc.hpp>

int pins[] = {18,19,21,22,23,13};
int num_pins = sizeof(pins) / sizeof(pins[0]);


BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;

// UUID for service and characteristic
#define SERVICE_UUID "02cb14bf-42e0-41bd-9d6d-68db02057e42"
#define CHARACTERISTIC_UUID "fef6efb5-fbf8-4d0a-a69f-57049733a87a"
#define LED_BUILTIN 5 // GPIO 핀 번호에 따라 변경할 수 있습니다.

struct S_Ble_Header_Req_Packet_V1 {
    uint32_t checkCode;
    uint8_t cmd;
    uint8_t parm[3];
};

struct S_Ble_Header_Res_Packet_V1 {
    uint32_t checkCode;
    uint8_t cmd;
    uint8_t parm[3];
};


class MyServerCallbacks : public BLEServerCallbacks
{ 
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();

        if (value.length() >= sizeof(S_Ble_Header_Req_Packet_V1)) {
            S_Ble_Header_Req_Packet_V1 *pkt = (S_Ble_Header_Req_Packet_V1 *)value.c_str();

            Serial.printf("check code : %d\n", pkt->checkCode);
            

            if (pkt->checkCode == 20230815) {
                
                // 체크 코드가 일치하면 명령 처리
                switch (pkt->cmd) {
                    case 0x01: //echo
                    {
                        S_Ble_Header_Res_Packet_V1 res;
                        res.checkCode = 20230815;
                        res.cmd = 0x11;
                        res.parm[0] = pkt->parm[0];
                        res.parm[1] = pkt->parm[1];
                        res.parm[2] = pkt->parm[2];

                        pCharacteristic->setValue((uint8_t *)&res, sizeof(res));
                        pCharacteristic->notify();
                        
                    }
                    break; 
                    case 0x02: //on
                    {
                        int pinIndex = pkt->parm[0];

                        // Serial.printf("pin index %d is on\n", pinIndex);

                        digitalWrite(pins[0], HIGH);

                        // Serial.printf("pin %d is on\n", pins[pinIndex]);
                        
                        S_Ble_Header_Res_Packet_V1 res;
                        res.checkCode = 20230815;
                        res.cmd = 0x02;
                        res.parm[0] = 0;
                        res.parm[1] = 0;
                        res.parm[2] = 0;

                        pCharacteristic->setValue((uint8_t *)&res, sizeof(res));
                        pCharacteristic->notify();                        
                    }
                    break;
                    case 0x03: //off
                    {
                        // int pinIndex = pkt->parm[0];

                        // Serial.printf("pin index %d is on\n", pinIndex);

                        digitalWrite(pins[0], LOW);

                        // Serial.printf("pin %d is off\n", pins[pinIndex]);
                        
                        S_Ble_Header_Res_Packet_V1 res;
                        res.checkCode = 20230815;
                        res.cmd = 0x03;

                        pCharacteristic->setValue((uint8_t *)&res, sizeof(res));
                        pCharacteristic->notify();                        
                    }
                    break;
                }
            }
        }
    }

   
    void onConnect(BLEServer *pServer)
    {
        deviceConnected = true;
        digitalWrite(LED_BUILTIN, HIGH);
        // digitalWrite(pins[0], HIGH);
        Serial.println("Client connected");
        pServer->getAdvertising()->stop(); // 클라이언트가 연결되면 광고 중지    

        // 환영 메시지 설정 및 알림 보내기
        pCharacteristic->setValue("Welcome to ESP32!");
        pCharacteristic->notify();    
        
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
    digitalWrite(LED_BUILTIN, LOW);

    for(int i=0; i<num_pins; i++){
        pinMode(pins[i], OUTPUT);
        digitalWrite(pins[i], LOW);
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

    // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
    pCharacteristic->addDescriptor(new BLE2902());

    // Start the service
    pService->start();

    // Start advertising
    pServer->getAdvertising()->start();
    Serial.printf("r02 Waiting a client connection to notify...%s \n", getChipID().c_str());
}

void loop()
{
    // if (deviceConnected)
    // {
    //     pCharacteristic->setValue("Hello from ESP32!");
    //     pCharacteristic->notify();
    //     delay(1000); // Send every second        
    // }
}
