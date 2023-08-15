#include <WiFi.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>


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

class MyCharateristicCallbacks: public BLECharacteristicCallbacks {
    
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.println("Received value from client:");
        for (int i = 0; i < value.length(); i++)
          Serial.print(value[i]);

        Serial.println();

        // Echo the value back by setting the same value
        // The client will read this value when it performs a read operation
        pCharacteristic->setValue(value);
        Serial.println("Set echo value for the client to read");
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
        pCharacteristic->setValue("Welcome to ESP32!");
        // pCharacteristic->notify();    
        
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

    // Create the BLE Device
    BLEDevice::init("ESP32_BLE" + std::string(getChipID().c_str()));
    // BLEDevice::init("MyESP32");
    
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
    
    // pCharacteristic->addDescriptor(new BLE2902()); // 알람 표시 기능활성화
    pCharacteristic->setCallbacks(new MyCharateristicCallbacks());

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
