#include <WiFi.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <etc.hpp>

// int pins[] = {18,19,21,22,23,13};
// int num_pins = sizeof(pins) / sizeof(pins[0]);


BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;

// UUID for service and characteristic
#define SERVICE_UUID "4448c051-e4e8-4681-b5d1-76372ce99167"
#define CHARACTERISTIC_UUID "f32c5d8a-7479-46a1-a69b-bda616225b99"
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


class MyServerCallbacks2 : public BLEServerCallbacks
{ 
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();
        Serial.printf("onWrite : %s\n", value.c_str());

        if (value == "hello") {
            // 환영 메시지 설정 및 알림 보내기
            pCharacteristic->setValue("ok from ESP32!");
            pCharacteristic->notify();
            // Serial.println("Response set to: ok from ESP32!");
        }
        else {
            pCharacteristic->setValue("world");
            pCharacteristic->notify();

        }
    }

    void onRead(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();
        Serial.printf("onRead : %s\n", value.c_str());
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

    // for(int i=0; i<num_pins; i++){
    //     pinMode(pins[i], OUTPUT);
    //     digitalWrite(pins[i], LOW);
    // }

    // Create the BLE Device
    BLEDevice::init("ESP32_BLE" + std::string(getChipID().c_str()));

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks2());

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
    Serial.printf("Waiting a client connection to notify...%s \n", getChipID().c_str());
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
