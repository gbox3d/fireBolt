# ESP32를 이용한 BLE 서버 및 서비스 관리 튜토리얼

이 튜토리얼에서는 ESP32를 사용하여 BLE(Bluetooth Low Energy) 서버를 구현하고 서비스를 관리하는 방법을 알아보겠습니다.

## 1. 필요한 라이브러리 포함

먼저 필요한 라이브러리들을 포함시킵니다:

```cpp
#include <Arduino.h>
#include <esp_system.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
```

이 라이브러리들은 ESP32의 BLE 기능을 사용하기 위해 필요합니다.

## 2. BLE 서버 설정

### 2.1 UUID 정의

BLE 서비스와 특성(Characteristic)을 식별하기 위한 UUID를 정의합니다:

```cpp
#define SERVICE_UUID "457556f0-842a-41ac-b777-cb4af6f47720"
#define CHARACTERISTIC_UUID "3c3ffbea-1856-460a-b0dd-b8a2a3a8352b"
```

### 2.2 BLE 서버 초기화

`setup()` 함수에서 BLE 서버를 초기화합니다:

```cpp
void setup() {
    BLEDevice::init("ESP32_BLE" + std::string(getChipID().c_str()));
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
}
```

여기서 `getChipID()`는 ESP32의 고유 ID를 반환하는 사용자 정의 함수입니다.

### 2.3 서비스 및 특성 생성

BLE 서비스와 특성을 생성합니다:

```cpp
BLEService *pService = pServer->createService(SERVICE_UUID);
pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE |
    BLECharacteristic::PROPERTY_NOTIFY |
    BLECharacteristic::PROPERTY_INDICATE
);
```

특성에 디스크립터를 추가하고 콜백을 설정합니다:

```cpp
pCharacteristic->addDescriptor(new BLE2902());
pCharacteristic->setCallbacks(new MyCharateristicCallbacks());
```

## 3. 콜백 구현

### 3.1 서버 콜백

`MyServerCallbacks` 클래스를 구현하여 연결 및 연결 해제 이벤트를 처리합니다:

```cpp
class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer *pServer) {
        deviceConnected = true;
        Serial.println("client connected");
        pServer->getAdvertising()->stop();
    }

    void onDisconnect(BLEServer *pServer) {
        deviceConnected = false;
        Serial.println("client disconnected");
        pServer->getAdvertising()->start();
    }
};
```

### 3.2 특성 콜백

`MyCharateristicCallbacks` 클래스를 구현하여 읽기 및 쓰기 작업을 처리합니다:

```cpp
class MyCharateristicCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();
        if (value.length() > 0) {
            Serial.println("Received BLE command:");
            Serial.println(value.c_str());
            // 명령 처리 및 응답 전송
        }
    }

    void onRead(BLECharacteristic *pCharacteristic) {
        Serial.println("BLE read : ");
        std::string value = pCharacteristic->getValue();
        Serial.println(value.c_str());
    }
};
```

## 4. 명령 처리

`ParseCmd` 함수를 구현하여 받은 명령을 처리하고 응답을 생성합니다:

```cpp
String ParseCmd(String _strLine) {
    // 명령 파싱 및 처리 로직
    // ...
    return response;
}
```

## 5. 광고 시작

서비스 시작 후 BLE 광고를 시작합니다:

```cpp
pService->start();
pServer->getAdvertising()->start();
```

## 6. 루프 실행

`loop()` 함수에서 태스크 스케줄러를 실행합니다:

```cpp
void loop() {
    g_ts.execute();
}
```

## 결론

이 튜토리얼에서는 ESP32를 사용하여 BLE 서버를 구현하고 서비스를 관리하는 방법을 살펴보았습니다. 이 구현을 통해 BLE 클라이언트와 통신하고, 명령을 수신하여 처리하며, 응답을 전송할 수 있습니다. 또한 연결 상태 변경 및 특성 읽기/쓰기 이벤트를 처리하는 방법도 알아보았습니다.