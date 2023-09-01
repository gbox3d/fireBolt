#include <WiFi.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <etc.hpp>

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;

// UUID for service and characteristic
#define SERVICE_UUID "02cb14bf-42e0-41bd-9d6d-68db02057e42"
#define CHARACTERISTIC_UUID "fef6efb5-fbf8-4d0a-a69f-57049733a87a"
#define LED_BUILTIN 5 // GPIO 핀 번호에 따라 변경할 수 있습니다.

class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        deviceConnected = true;
        digitalWrite(LED_BUILTIN, HIGH);
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
        Serial.println("Client disconnected");
        pServer->getAdvertising()->start(); // 클라이언트가 연결 해제되면 광고 다시 시작
    }
};

void setup()
{
    Serial.begin(115200);

    pinMode(LED_BUILTIN, OUTPUT); // 내장 LED를 출력 모드로 설정
    digitalWrite(LED_BUILTIN, LOW);

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
