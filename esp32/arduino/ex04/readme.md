## preipheral 만들기

```c++
 // Create the BLE Device
BLEDevice::init("ESP32_BLE" + std::string(getChipID().c_str()));

// Create the BLE Server
pServer = BLEDevice::createServer();
pServer->setCallbacks(new MyServerCallbacks());
```

init 로 디바이스 이름을 지정한다.  
그리고 서버를 생성한다.  
서버의 이밴트 처리를 위하여 BLEServerCallbacks 에서 상속받은 콜백 객체를 등록한다.  
본 예제 에서는 onConnect, onDisconnect, 구현했다.  


## central 실행
```yaml
mac : 24:0A:C4:88:C2:7E
char_uuid : beb5483e-36e1-4688-b7f5-ea07361b26a8
```

central/ 위와 같이 config.yaml 파일을 만들어 주세요.(주소는 달라질수있음)  